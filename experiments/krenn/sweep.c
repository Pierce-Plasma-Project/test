/*
 * Exhaustive sweep over all 3-colorings of the edges of a support graph
 * (monochromatic edges: each edge shows one color at both endpoints),
 * filtering for candidate Krenn-Gu counterexamples with d = 3 and
 * nonzero complex weights.
 *
 * Necessary conditions enforced (both exact, both combinatorial):
 *   P1: for each color c in {0,1,2} there is a perfect matching whose
 *       edges are all colored c (else the monochromatic weight is an
 *       empty sum = 0, never the required common nonzero value);
 *   P2: every induced NON-monochromatic vertex coloring is induced by
 *       at least 2 perfect matchings (a class with exactly one matching
 *       has weight = a product of nonzero weights != 0, violating the
 *       cancellation requirement).
 *
 * Survivors are printed (base-3 coloring code) for the exact
 * algebraic stage (analyze.py). If no coloring survives, the sweep
 * itself is a proof for this support (mono edges, nonzero weights).
 *
 * Usage: sweep INSTANCE_FILE
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    int D = (argc > 2) ? atoi(argv[2]) : 3;
    FILE *f = fopen(argv[1], "r");
    if (!f) { perror(argv[1]); return 1; }
    int E, NPM, N;
    if (fscanf(f, "%d %d %d", &E, &NPM, &N) != 3) return 1;
    int half = N / 2;
    int (*pmE)[16] = malloc(NPM * sizeof *pmE);   /* edges of each PM  */
    int (*pmV)[16] = malloc(NPM * sizeof *pmV);   /* vertex->edge map  */
    for (int m = 0; m < NPM; m++) {
        for (int k = 0; k < half; k++) fscanf(f, "%d", &pmE[m][k]);
        for (int v = 0; v < N; v++)  fscanf(f, "%d", &pmV[m][v]);
    }
    fclose(f);

    long total = 1;
    for (int i = 0; i < E; i++) total *= D;

    int col[32];
    long survivors = 0, p1pass = 0;
    /* kappa codes fit in 3^N; count multiplicities per class */
    int nk = 1; for (int v = 0; v < N; v++) nk *= D;
    unsigned char *cnt = calloc(nk, 1);
    int kap[4096]; int mono[3];
    mono[0] = 0; mono[1] = 0; mono[2] = 0;
    for (int v = 0; v < N; v++) { mono[1] = mono[1]*1; }
    /* mono kappa codes: c * (3^N-1)/2 since digits all c */
    int rep = (nk - 1) / (D - 1);                 /* 111...1 in base D */
    int monoc[8]; for (int c = 0; c < D; c++) monoc[c] = c * rep;

    memset(col, 0, sizeof col);
    for (long it = 0; it < total; it++) {
        /* P1: monochromatic PM in each color */
        int have[8] = {0}, nhave = 0;
        for (int m = 0; m < NPM && nhave < D; m++) {
            int c0 = col[pmE[m][0]], ok = 1;
            for (int k = 1; k < half; k++)
                if (col[pmE[m][k]] != c0) { ok = 0; break; }
            if (ok && !have[c0]) { have[c0] = 1; nhave++; }
        }
        if (nhave == D) {
            p1pass++;
            /* P2: every mixed class has >= 2 PMs */
            int nkap = 0, ok = 1;
            for (int m = 0; m < NPM; m++) {
                int code = 0;
                for (int v = N - 1; v >= 0; v--)
                    code = code * D + col[pmV[m][v]];
                kap[nkap++] = code;
                cnt[code]++;
            }
            for (int m = 0; m < nkap && ok; m++) {
                int code = kap[m];
                int ismono = 0;
                for (int c = 0; c < D; c++) if (code == monoc[c]) ismono = 1;
                if (cnt[code] == 1 && !ismono) ok = 0;
            }
            for (int m = 0; m < nkap; m++) cnt[kap[m]] = 0;
            if (ok) { survivors++;
                printf("S ");
                for (int i = 0; i < E; i++) printf("%d", col[i]);
                printf("\n");
            }
        }
        /* increment base-3 counter */
        for (int i = 0; i < E; i++) {
            if (++col[i] < D) break;
            col[i] = 0;
        }
    }
    fprintf(stderr, "total %ld  P1-pass %ld  survivors %ld\n",
            total, p1pass, survivors);
    return 0;
}
