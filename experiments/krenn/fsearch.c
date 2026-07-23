/*
 * Exhaustive finite-field search for GHZ-system solutions under a
 * symmetry ansatz. Reads monomial tables from ghz_ansatz.py; enumerates
 * all assignments V in F_q^NV (q prime, or q=4 for GF(4), which
 * contains the cube roots of unity — the natural d=3 alphabet).
 * Conditions: every mixed class sums to 0; the d mono classes share one
 * common nonzero value. Mixed classes are checked with early exit;
 * class evaluation order puts a spread of mixed classes first.
 *
 * Usage: fsearch TABLES Q [shard nshards]
 * Survivor lines: "V v0 v1 ... v_{NV-1}"
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int NV, NC, NPM, n, d, half, Q;
static int *cmono, *cclr;
static unsigned short *mons;              /* NC*NPM*half ids */
static int add[16][16], mul[16][16];

static void field_init(void)
{
    if (Q == 4) {                          /* GF(4): {0,1,w,w+1}, xor add */
        int mt[4][4] = {{0,0,0,0},{0,1,2,3},{0,2,3,1},{0,3,1,2}};
        for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) {
            add[i][j] = i ^ j;
            mul[i][j] = mt[i][j];
        }
    } else {
        for (int i = 0; i < Q; i++) for (int j = 0; j < Q; j++) {
            add[i][j] = (i + j) % Q;
            mul[i][j] = (i * j) % Q;
        }
    }
}

static inline int class_sum(int ci, const int *V)
{
    const unsigned short *base = mons + (long)ci * NPM * half;
    int s = 0;
    for (int m = 0; m < NPM; m++) {
        const unsigned short *mo = base + m * half;
        int p = V[mo[0]];
        for (int t = 1; t < half; t++) p = mul[p][V[mo[t]]];
        s = add[s][p];
    }
    return s;
}

int main(int argc, char **argv)
{
    FILE *f = fopen(argv[1], "r");
    if (!f) { perror(argv[1]); return 1; }
    Q = atoi(argv[2]);
    long shard = argc > 3 ? atol(argv[3]) : 0;
    long nsh = argc > 4 ? atol(argv[4]) : 1;
    if (fscanf(f, "%d %d %d %d %d", &NV, &NC, &NPM, &n, &d) != 5) return 1;
    half = n / 2;
    cmono = malloc(NC * sizeof(int));
    cclr = malloc(NC * sizeof(int));
    mons = malloc((long)NC * NPM * half * sizeof(unsigned short));
    for (int c = 0; c < NC; c++) {
        if (fscanf(f, "%d %d", &cmono[c], &cclr[c]) != 2) return 1;
        for (int m = 0; m < NPM; m++)
            for (int t = 0; t < half; t++)
                if (fscanf(f, "%hu",
                           &mons[((long)c * NPM + m) * half + t]) != 1)
                    return 1;
    }
    fclose(f);
    field_init();

    /* evaluation order: interleave mixed classes for early discrimination,
       then mono classes, then the rest of the mixed ones */
    int *order = malloc(NC * sizeof(int)), no = 0;
    for (int c = 0; c < NC; c += 97) if (!cmono[c]) order[no++] = c;
    for (int c = 0; c < NC; c++) if (cmono[c]) order[no++] = c;
    for (int c = 0; c < NC; c++) {
        if (cmono[c]) continue;
        int seen = 0;
        for (int i = 0; i < no && !seen; i++) if (order[i] == c) seen = 1;
        if (!seen) order[no++] = c;
    }

    long total = 1;
    for (int i = 0; i < NV; i++) total *= Q;
    long lo = total / nsh * shard, hi = shard == nsh-1 ? total
                                        : total / nsh * (shard + 1);
    int V[64];
    long survivors = 0;
    for (long it = lo; it < hi; it++) {
        long x = it;
        for (int i = 0; i < NV; i++) { V[i] = x % Q; x /= Q; }
        int monoval = -1, ok = 1;
        for (int oi = 0; oi < no && ok; oi++) {
            int c = order[oi];
            int s = class_sum(c, V);
            if (cmono[c]) {
                if (s == 0) ok = 0;
                else if (monoval < 0) monoval = s;
                else if (s != monoval) ok = 0;
            } else if (s != 0) ok = 0;
        }
        if (ok) {
            survivors++;
            printf("V");
            for (int i = 0; i < NV; i++) printf(" %d", V[i]);
            printf("\n");
            fflush(stdout);
        }
    }
    fprintf(stderr, "Q=%d NV=%d range [%ld,%ld) survivors %ld\n",
            Q, NV, lo, hi, survivors);
    return 0;
}
