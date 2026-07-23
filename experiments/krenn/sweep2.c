/*
 * Exhaustive sweep with a bichromatic budget k: exactly k edges carry an
 * ordered pair of DISTINCT colors (color shown at u-end, at v-end); the
 * remaining E-k edges are monochromatic. All d^(E-k) * C(E,k) * (d(d-1))^k
 * colorings are enumerated.
 *
 * Filters (exact necessary conditions, nonzero weights):
 *   P1: for each color c, some perfect matching consists entirely of
 *       (c,c) edges (bichromatic edges never qualify);
 *   P2: every non-monochromatic induced vertex coloring class contains
 *       at least 2 perfect matchings.
 * Survivors go to stdout for the exact algebraic stage.
 *
 * Usage: sweep2 INSTANCE D K
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int E, NPM, N, D, K, half;
static int pmE[32][16], pmV[32][16], pmEnd[32][16];
static int colU[32], colV[32];        /* color at u-end / v-end */
static long survivors, p1pass, total;
static unsigned char *cnt;
static int monoc[8], nk;

static void test(void)
{
    total++;
    int have[8] = {0}, nhave = 0;
    for (int m = 0; m < NPM && nhave < D; m++) {
        int e0 = pmE[m][0];
        if (colU[e0] != colV[e0]) continue;
        int c0 = colU[e0], ok = 1;
        for (int t = 1; t < half; t++) {
            int e = pmE[m][t];
            if (colU[e] != c0 || colV[e] != c0) { ok = 0; break; }
        }
        if (ok && !have[c0]) { have[c0] = 1; nhave++; }
    }
    if (nhave < D) return;
    p1pass++;
    int kap[32], ok = 1;
    for (int m = 0; m < NPM; m++) {
        int code = 0;
        for (int v = N - 1; v >= 0; v--) {
            int e = pmV[m][v];
            int c = pmEnd[m][v] ? colV[e] : colU[e];
            code = code * D + c;
        }
        kap[m] = code;
        cnt[code]++;
    }
    for (int m = 0; m < NPM && ok; m++) {
        int ismono = 0;
        for (int c = 0; c < D; c++) if (kap[m] == monoc[c]) ismono = 1;
        if (cnt[kap[m]] == 1 && !ismono) ok = 0;
    }
    for (int m = 0; m < NPM; m++) cnt[kap[m]] = 0;
    if (ok) {
        survivors++;
        printf("S ");
        for (int i = 0; i < E; i++) printf("%d%d.", colU[i], colV[i]);
        printf("\n");
    }
}

/* enumerate ordered distinct pairs on the chosen bichromatic edges,
   then all mono colorings of the rest */
static int bic[8];

static void mono_loop(int mono_edges[], int nm)
{
    long tot = 1;
    for (int i = 0; i < nm; i++) tot *= D;
    int c[32]; memset(c, 0, sizeof c);
    for (long it = 0; it < tot; it++) {
        for (int i = 0; i < nm; i++)
            colU[mono_edges[i]] = colV[mono_edges[i]] = c[i];
        test();
        for (int i = 0; i < nm; i++) { if (++c[i] < D) break; c[i] = 0; }
    }
}

static void pair_loop(int idx, int mono_edges[], int nm)
{
    if (idx == K) { mono_loop(mono_edges, nm); return; }
    int e = bic[idx];
    for (int a = 0; a < D; a++) for (int b = 0; b < D; b++) {
        if (a == b) continue;
        colU[e] = a; colV[e] = b;
        pair_loop(idx + 1, mono_edges, nm);
    }
}

static void choose(int start, int got)
{
    if (got == K) {
        int mono_edges[32], nm = 0, isb;
        for (int i = 0; i < E; i++) {
            isb = 0;
            for (int j = 0; j < K; j++) if (bic[j] == i) isb = 1;
            if (!isb) mono_edges[nm++] = i;
        }
        pair_loop(0, mono_edges, nm);
        return;
    }
    for (int i = start; i < E; i++) { bic[got] = i; choose(i + 1, got + 1); }
}

int main(int argc, char **argv)
{
    FILE *f = fopen(argv[1], "r");
    if (!f) { perror(argv[1]); return 1; }
    D = atoi(argv[2]); K = atoi(argv[3]);
    if (fscanf(f, "%d %d %d", &E, &NPM, &N) != 3) return 1;
    half = N / 2;
    for (int m = 0; m < NPM; m++) {
        for (int t = 0; t < half; t++) if (!fscanf(f, "%d", &pmE[m][t])) return 1;
        for (int v = 0; v < N; v++) if (!fscanf(f, "%d", &pmV[m][v])) return 1;
        for (int v = 0; v < N; v++) if (!fscanf(f, "%d", &pmEnd[m][v])) return 1;
    }
    fclose(f);
    if (3 * D > E - K) {
        fprintf(stderr, "edge-count prune: 3d=%d > E-k=%d -> IMPOSSIBLE, "
                        "0 colorings viable\n", 3 * D, E - K);
        return 0;
    }
    nk = 1; for (int v = 0; v < N; v++) nk *= D;
    cnt = calloc(nk, 1);
    int rep = (nk - 1) / (D - 1);
    for (int c = 0; c < D; c++) monoc[c] = c * rep;
    choose(0, 0);
    fprintf(stderr, "total %ld  P1-pass %ld  survivors %ld\n",
            total, p1pass, survivors);
    return 0;
}
