/*
 * Fast focused search over cross-block amplitudes (intra-block pinned to
 * the K4 GHZ solution, baked into monomial coefficients). Reads cross_tab.txt.
 * Field F_p (p prime, cube roots of unity live in p=7,13,19,...).
 * cost = 100*(mono violations) + (violated cross-dependent mixed classes).
 * cost 0 => cross amplitudes kill the six-term obstruction => lift & verify.
 *
 * Usage: crosssearch P SEED [restarts moves]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int NV, NC, P;
static int *cmono, *cclr;
static int *tcnt;              /* terms per class */
static int **tcoef;           /* [class][term] constant coeff */
static int **tlen;            /* [class][term] #cross vars */
static int ***tvar;           /* [class][term][k] cross var id */
static int *S, *V;
static int monoidx[8], nmono;

static unsigned long rs;
static inline unsigned long rnd(void){rs^=rs<<13;rs^=rs>>7;rs^=rs<<17;return rs;}

static int class_sum(int ci)
{
    long s = 0;
    for (int t = 0; t < tcnt[ci]; t++) {
        long p = tcoef[ci][t] % P;
        for (int k = 0; k < tlen[ci][t]; k++) { p = (p * V[tvar[ci][t][k]]) % P; if(!p) break; }
        s = (s + p) % P;
    }
    return (int)s;
}

static int cost(void)
{
    int c = 0, mv = -1;
    for (int ci = 0; ci < NC; ci++) {
        S[ci] = class_sum(ci);
        if (cmono[ci]) {
            if (S[ci] == 0) c += 100;
            else if (mv < 0) mv = S[ci];
            else if (S[ci] != mv) c += 100;
        } else if (S[ci]) c++;
    }
    return c;
}

int main(int argc, char **argv)
{
    P = atoi(argv[1]);
    rs = argc > 2 ? atol(argv[2]) : 1;
    long restarts = argc > 3 ? atol(argv[3]) : 30;
    long moves = argc > 4 ? atol(argv[4]) : 4000;
    FILE *f = fopen("cross_tab.txt", "r");
    if (!f) { perror("cross_tab.txt"); return 1; }
    if (fscanf(f, "%d %d", &NV, &NC) != 2) return 1;
    cmono = malloc(NC*sizeof(int)); cclr = malloc(NC*sizeof(int));
    tcnt = malloc(NC*sizeof(int));
    tcoef = malloc(NC*sizeof(int*)); tlen = malloc(NC*sizeof(int*));
    tvar = malloc(NC*sizeof(int**));
    for (int ci = 0; ci < NC; ci++) {
        if (fscanf(f, "%d %d", &cmono[ci], &tcnt[ci]) != 2) return 1;
        if (cmono[ci]) monoidx[nmono++] = ci;
        tcoef[ci] = malloc(tcnt[ci]*sizeof(int));
        tlen[ci] = malloc(tcnt[ci]*sizeof(int));
        tvar[ci] = malloc(tcnt[ci]*sizeof(int*));
        for (int t = 0; t < tcnt[ci]; t++) {
            if (fscanf(f, "%d %d", &tcoef[ci][t], &tlen[ci][t]) != 2) return 1;
            tvar[ci][t] = malloc(tlen[ci][t]*sizeof(int));
            for (int k = 0; k < tlen[ci][t]; k++)
                if (fscanf(f, "%d", &tvar[ci][t][k]) != 1) return 1;
        }
    }
    fclose(f);
    S = malloc(NC*sizeof(int)); V = calloc(NV, sizeof(int));

    int best = 1<<30;
    for (long r = 0; r < restarts; r++) {
        for (int i = 0; i < NV; i++) V[i] = (rnd()%100 < 4) ? rnd()%P : 0;
        int c = cost();
        for (long mv = 0; mv < moves && c; mv++) {
            int i = rnd()%NV, old = V[i];
            V[i] = rnd()%P;
            int c2 = cost();
            if (c2 <= c || rnd()%100 < 5) c = c2; else V[i] = old;
        }
        if (c < best) { best = c; fprintf(stderr,"restart %ld best %d\n",r,best); }
        if (c == 0) {
            printf("V");
            for (int i = 0; i < NV; i++) printf(" %d", V[i]);
            printf("\n");
            return 0;
        }
    }
    fprintf(stderr, "no cross-block solution; best cost %d\n", best);
    return 2;
}
