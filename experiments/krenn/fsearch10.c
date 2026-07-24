/*
 * Exhaustive finite-field hunt at n = 10 with on-the-fly evaluation
 * (the class/matching tables are too large to precompute at n=10:
 * 3^10 classes x 945 matchings). Circulant color-cyclic ansatz built
 * in: W_{uv}[a][b] = V[(dist(u,v)-1)*3 + (b-a mod 3)], NV = 15.
 *
 * For each assignment V in F_q^15: check the 3 mono classes (equal,
 * nonzero), then all mixed classes with early exit. Mixed classes are
 * streamed in a fixed pseudo-shuffled order for fast rejection.
 *
 * Usage: fsearch10 Q [shard nshards]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 10
#define D 3
#define HALF 5
static int NPM;
static int pmu[1024][HALF], pmv[1024][HALF];
static int Q;
static int fadd[16][16], fmul[16][16];
static int NV = 15;

static void gen_pms(int used, int *cu, int *cv, int k)
{
    if (used == (1 << N) - 1) {
        for (int t = 0; t < HALF; t++) { pmu[NPM][t] = cu[t]; pmv[NPM][t] = cv[t]; }
        NPM++;
        return;
    }
    int v0 = 0;
    while (used & (1 << v0)) v0++;
    for (int w = v0 + 1; w < N; w++) {
        if (used & (1 << w)) continue;
        cu[k] = v0; cv[k] = w;
        gen_pms(used | (1 << v0) | (1 << w), cu, cv, k + 1);
    }
}

static inline int vidx(int u, int v, int a, int b)
{
    int dd = u > v ? u - v : v - u;
    if (dd > N / 2) dd = N - dd;
    return (dd - 1) * D + ((b - a + D) % D);
}

int main(int argc, char **argv)
{
    Q = atoi(argv[1]);
    long shard = argc > 2 ? atol(argv[2]) : 0;
    long nsh = argc > 3 ? atol(argv[3]) : 1;
    if (Q == 4) {
        int mt[4][4] = {{0,0,0,0},{0,1,2,3},{0,2,3,1},{0,3,1,2}};
        for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++)
            { fmul[i][j] = mt[i][j]; fadd[i][j] = i ^ j; }
    } else
        for (int i = 0; i < Q; i++) for (int j = 0; j < Q; j++)
            { fmul[i][j] = (i * j) % Q; fadd[i][j] = (i + j) % Q; }
    int cu[HALF], cv[HALF];
    gen_pms(0, cu, cv, 0);

    /* enumerate kappa: mono first (3), then mixed pseudo-shuffled */
    static int kap[59049][N];
    int nkap = 0;
    for (long code = 0; code < 59049; code++) {
        long x = code;
        for (int v = 0; v < N; v++) { kap[nkap][v] = x % 3; x /= 3; }
        nkap++;
    }
    static int order[59049];
    int no = 0;
    /* mono codes: all-same */
    long monocode[3] = {0, (59049 - 1) / 2, 59049 - 1};
    for (int c = 0; c < 3; c++) order[no++] = monocode[c];
    for (long code = 0, step = 0; step < 59049; step++) {
        code = (code + 24571) % 59049;             /* coprime stride */
        if (code != monocode[0] && code != monocode[1] && code != monocode[2])
            order[no++] = code;
    }

    /* collapsed filter tables: first NFILT mixed classes + 3 mono.
       Each class's 945 monomials collapse to distinct sorted var-tuples
       with multiplicity (coefficient mod Q; zero-coef monomials drop). */
    #define NFILT 19
    static int fmon[NFILT + 3][4800][6];   /* [k][j] = {coef, ids...} */
    static int fcnt[NFILT + 3];
    int fclass[NFILT + 3];
    for (int c = 0; c < 3; c++) fclass[c] = order[c];         /* mono */
    for (int j = 0; j < NFILT; j++) fclass[3 + j] = order[3 + j];
    for (int fi = 0; fi < NFILT + 3; fi++) {
        int *k = kap[fclass[fi]];
        static int tup[945][6];
        for (int m = 0; m < NPM; m++) {
            int ids[HALF];
            for (int t = 0; t < HALF; t++)
                ids[t] = vidx(pmu[m][t], pmv[m][t], k[pmu[m][t]], k[pmv[m][t]]);
            for (int a = 0; a < HALF; a++) for (int b = a + 1; b < HALF; b++)
                if (ids[b] < ids[a]) { int tt = ids[a]; ids[a] = ids[b]; ids[b] = tt; }
            for (int t = 0; t < HALF; t++) tup[m][t] = ids[t];
        }
        fcnt[fi] = 0;
        for (int m = 0; m < NPM; m++) {
            int found = -1;
            for (int j = 0; j < fcnt[fi]; j++) {
                int same = 1;
                for (int t = 0; t < HALF; t++)
                    if (fmon[fi][j][t + 1] != tup[m][t]) { same = 0; break; }
                if (same) { found = j; break; }
            }
            if (found >= 0) fmon[fi][found][0] = fadd[fmon[fi][found][0]][1];
            else {
                fmon[fi][fcnt[fi]][0] = 1 % Q;
                for (int t = 0; t < HALF; t++)
                    fmon[fi][fcnt[fi]][t + 1] = tup[m][t];
                fcnt[fi]++;
            }
        }
        /* drop zero coefficients */
        int w = 0;
        for (int j = 0; j < fcnt[fi]; j++)
            if (fmon[fi][j][0]) { memcpy(fmon[fi][w], fmon[fi][j], 24); w++; }
        fcnt[fi] = w;
    }
    long fsum = 0;
    for (int fi = 0; fi < NFILT + 3; fi++) fsum += fcnt[fi];
    fprintf(stderr, "filter classes collapsed: avg %ld monomials (from %d)\n",
            fsum / (NFILT + 3), NPM);

    long total = 1;
    for (int i = 0; i < NV; i++) total *= Q;
    long lo = total / nsh * shard;
    long hi = shard == nsh - 1 ? total : total / nsh * (shard + 1);
    int V[32];
    long survivors = 0;
    for (long it = lo; it < hi; it++) {
        long x = it;
        for (int i = 0; i < NV; i++) { V[i] = x % Q; x /= Q; }
        int ok = 1, monoval = -1;
        /* stage 1: collapsed filters — 2 mixed, 3 mono, rest mixed */
        static const int forder[NFILT + 3] =
            {3, 4, 0, 1, 2, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
             15, 16, 17, 18, 19, 20, 21};
        for (int q2 = 0; q2 < NFILT + 3 && ok; q2++) {
            int fi = forder[q2];
            int s = 0;
            for (int j = 0; j < fcnt[fi]; j++) {
                const int *mo = fmon[fi][j];
                int p = mo[0];
                for (int t = 1; t <= HALF && p; t++) p = fmul[p][V[mo[t]]];
                s = fadd[s][p];
            }
            if (fi < 3) {
                if (s == 0) ok = 0;
                else if (monoval < 0) monoval = s;
                else if (s != monoval) ok = 0;
            } else if (s != 0) ok = 0;
        }
        /* stage 2: full on-the-fly check for the rare survivors */
        for (int oi = 3 + NFILT; oi < no && ok; oi++) {
            int *k = kap[order[oi]];
            int s = 0;
            for (int m = 0; m < NPM; m++) {
                int p = V[vidx(pmu[m][0], pmv[m][0], k[pmu[m][0]], k[pmv[m][0]])];
                for (int t = 1; t < HALF && p; t++)
                    p = fmul[p][V[vidx(pmu[m][t], pmv[m][t],
                                       k[pmu[m][t]], k[pmv[m][t]])]];
                s = fadd[s][p];
            }
            if (s != 0) ok = 0;
        }
        if (ok) {
            survivors++;
            printf("V");
            for (int i = 0; i < NV; i++) printf(" %d", V[i]);
            printf("\n");
            fflush(stdout);
        }
    }
    fprintf(stderr, "n=10 cc Q=%d NPM=%d range [%ld,%ld) survivors %ld\n",
            Q, NPM, lo, hi, survivors);
    return 0;
}
