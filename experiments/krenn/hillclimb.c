/*
 * Ansatz-free stochastic search for GHZ-system solutions over GF(4).
 * Works on the pair-full tables (every pair of K_n carries a full dxd
 * matrix; variables = all entries). WalkSAT-flavored: greedy descent on
 * the number of violated classes with noise moves and random restarts.
 * Class sums are maintained incrementally via cached monomial products
 * (GF(4) addition is XOR, so removing a stale product is the same op as
 * adding the fresh one).
 *
 * cost = #(mixed classes with sum != 0)
 *      + #(mono classes with sum == 0)
 *      + #(mono classes with sum != first nonzero mono sum)
 * cost == 0  <=>  exact GHZ solution over GF(4)  (print and exit).
 *
 * Usage: hillclimb TABLES SEED [max_restarts] [moves_per_restart]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int NV, NC, NPM, n, d, half;
static int *cmono;
static unsigned short *mons;             /* NC*NPM*half */
static unsigned char *prod;              /* cached product per monomial */
static unsigned char *S;                 /* class sums */
static int *inc, *incoff;                /* var -> monomial indices */
static int V[512];
static int Q = 4;
static int fmul[16][16], fadd[16][16], fsub[16][16];
static void field_init(void) {
    if (Q == 4) {
        int mt[4][4] = {{0,0,0,0},{0,1,2,3},{0,2,3,1},{0,3,1,2}};
        for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) {
            fmul[i][j] = mt[i][j]; fadd[i][j] = i ^ j; fsub[i][j] = i ^ j;
        }
    } else {
        for (int i = 0; i < Q; i++) for (int j = 0; j < Q; j++) {
            fmul[i][j] = (i * j) % Q; fadd[i][j] = (i + j) % Q;
            fsub[i][j] = (i - j + Q) % Q;
        }
    }
}

static unsigned long rng;
static inline unsigned long rnd(void)
{ rng ^= rng << 13; rng ^= rng >> 7; rng ^= rng << 17; return rng; }

static int monoidx[8], nmono;

static int cost(void)
{
    int c = 0, mv = -1;
    for (int i = 0; i < nmono; i++) {
        int s = S[monoidx[i]];
        if (s == 0) c += 200;           /* zero-collapse must never win */
        else if (mv < 0) mv = s;
        else if (s != mv) c += 200;
    }
    for (int ci = 0; ci < NC; ci++)
        if (!cmono[ci] && S[ci]) c++;
    return c;
}

static void full_eval(void)
{
    memset(S, 0, NC);
    for (int ci = 0; ci < NC; ci++)
        for (int m = 0; m < NPM; m++) {
            long mi = (long)ci * NPM + m;
            const unsigned short *mo = mons + mi * half;
            int p = V[mo[0]];
            for (int t = 1; t < half; t++) p = fmul[p][V[mo[t]]];
            prod[mi] = p;
            S[ci] = fadd[S[ci]][p];
        }
}

static void set_var(int v, int val)
{
    V[v] = val;
    for (int k = incoff[v]; k < incoff[v + 1]; k++) {
        long mi = inc[k];
        const unsigned short *mo = mons + mi * half;
        int p = V[mo[0]];
        for (int t = 1; t < half; t++) p = fmul[p][V[mo[t]]];
        S[mi / NPM] = fadd[fsub[S[mi / NPM]][prod[mi]]][p];
        prod[mi] = p;
    }
}

int main(int argc, char **argv)
{
    FILE *f = fopen(argv[1], "r");
    if (!f) { perror(argv[1]); return 1; }
    rng = argc > 2 ? atol(argv[2]) : 12345;
    long restarts = argc > 3 ? atol(argv[3]) : 1000;
    long moves = argc > 4 ? atol(argv[4]) : 200000;
    if (argc > 5) Q = atoi(argv[5]);
    field_init();
    if (fscanf(f, "%d %d %d %d %d", &NV, &NC, &NPM, &n, &d) != 5) return 1;
    half = n / 2;
    cmono = malloc(NC * sizeof(int));
    mons = malloc((long)NC * NPM * half * sizeof(unsigned short));
    prod = malloc((long)NC * NPM);
    S = malloc(NC);
    for (int ci = 0; ci < NC; ci++) {
        int clr;
        if (fscanf(f, "%d %d", &cmono[ci], &clr) != 2) return 1;
        if (cmono[ci]) monoidx[nmono++] = ci;
        for (int m = 0; m < NPM; m++)
            for (int t = 0; t < half; t++)
                if (fscanf(f, "%hu",
                           &mons[((long)ci * NPM + m) * half + t]) != 1)
                    return 1;
    }
    fclose(f);

    /* incidence lists (dedup within a monomial) */
    long nmon = (long)NC * NPM;
    int *deg = calloc(NV + 1, sizeof(int));
    for (long mi = 0; mi < nmon; mi++) {
        const unsigned short *mo = mons + mi * half;
        for (int t = 0; t < half; t++) {
            int dup = 0;
            for (int s = 0; s < t; s++) if (mo[s] == mo[t]) dup = 1;
            if (!dup) deg[mo[t]]++;
        }
    }
    incoff = malloc((NV + 1) * sizeof(int));
    incoff[0] = 0;
    for (int v = 0; v < NV; v++) incoff[v + 1] = incoff[v] + deg[v];
    inc = malloc(incoff[NV] * sizeof(int));
    int *fill = calloc(NV, sizeof(int));
    for (long mi = 0; mi < nmon; mi++) {
        const unsigned short *mo = mons + mi * half;
        for (int t = 0; t < half; t++) {
            int dup = 0;
            for (int s = 0; s < t; s++) if (mo[s] == mo[t]) dup = 1;
            if (!dup) inc[incoff[mo[t]] + fill[mo[t]]++] = mi;
        }
    }

    int bestever = 1 << 30;
    for (long r = 0; r < restarts; r++) {
        /* scaffold init: three edge-disjoint PMs of K8 carry the mono
           backbone (entries W[c][c]=1); sparse random elsewhere */
        for (int v = 0; v < NV; v++)
            V[v] = (rnd() % 100 < 12) ? (int)(rnd() % (Q - 1)) + 1 : 0;
        if (NV == 252) {
            int pm[3][4][2] = {
                {{0,1},{2,3},{4,5},{6,7}},
                {{0,2},{1,3},{4,6},{5,7}},
                {{0,3},{1,2},{4,7},{5,6}}};
            for (int c3 = 0; c3 < 3; c3++)
                for (int e = 0; e < 4; e++) {
                    int u = pm[c3][e][0], v2 = pm[c3][e][1];
                    int pi = u * (15 - u) / 2 + (v2 - u - 1);
                    V[pi * 9 + c3 * 3 + c3] = 1;
                }
        }
        full_eval();
        int c = cost();
        for (long mv = 0; mv < moves && c; mv++) {
            /* focused move: pick a violated class, then a variable in it */
            int v;
            int tries = 0;
            for (;;) {
                int ci = rnd() % NC;
                int bad = cmono[ci] ? (S[ci] == 0) : (S[ci] != 0);
                if (bad || ++tries > 64) {
                    long mi = (long)ci * NPM + rnd() % NPM;
                    v = mons[mi * half + rnd() % half];
                    break;
                }
            }
            int old = V[v];
            /* try the best of the 3 alternative values (greedy),
               with occasional pure-random noise */
            if (rnd() % 100 < 15) {
                int nu = rnd() % Q;
                if (nu == old) continue;
                set_var(v, nu);
                int c2 = cost();
                if (c2 <= c + 2) c = c2; else set_var(v, old);
            } else {
                int bestv = old, bestc = c;
                for (int nu = 0; nu < Q; nu++) {
                    if (nu == old) continue;
                    set_var(v, nu);
                    int c2 = cost();
                    if (c2 < bestc) { bestc = c2; bestv = nu; }
                }
                set_var(v, bestv);
                c = bestc;
            }
        }
        if (c < bestever) {
            bestever = c;
            fprintf(stderr, "restart %ld: best cost %d\n", r, bestever);
        }
        if (c == 0) {
            printf("V");
            for (int v = 0; v < NV; v++) printf(" %d", V[v]);
            printf("\n");
            fflush(stdout);
            return 0;
        }
    }
    fprintf(stderr, "no solution found; best cost %d\n", bestever);
    return 2;
}
