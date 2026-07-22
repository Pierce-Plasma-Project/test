/*
 * A minimal DIMACS DPLL solver: unit propagation + chronological
 * backtracking, static branching order by variable occurrence count.
 * No clause learning, no restarts — deliberately naive, as the baseline
 * against which modern CDCL (CaDiCaL, via sat_check.py) is compared.
 *
 * Usage: solver FILE.cnf [decision_cap]
 * Exit codes: 10 = SAT, 20 = UNSAT, 30 = decision cap exceeded.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int nv, nc;
static int **cls, *clen;
static int *assign;            /* 0 unset, +1 true, -1 false */
static int *order;             /* branching order             */
static long decisions, cap = 50000000L;
static int capped;

static int lit_val(int lit)
{
    int a = assign[abs(lit)];
    return a == 0 ? 0 : (lit > 0 ? a : -a);
}

static int solve(void)
{
    if (capped) return 0;
    int *trail = malloc(nv * sizeof(int)), ntrail = 0, changed = 1;
    while (changed) {
        changed = 0;
        for (int c = 0; c < nc; c++) {
            int unset = 0, sat = 0, last = 0;
            for (int k = 0; k < clen[c]; k++) {
                int v = lit_val(cls[c][k]);
                if (v == 1) { sat = 1; break; }
                if (v == 0) { unset++; last = cls[c][k]; }
            }
            if (sat) continue;
            if (unset == 0) {                       /* conflict */
                while (ntrail) assign[abs(trail[--ntrail])] = 0;
                free(trail); return 0;
            }
            if (unset == 1) {                       /* unit */
                assign[abs(last)] = last > 0 ? 1 : -1;
                trail[ntrail++] = last;
                changed = 1;
            }
        }
    }
    int var = 0;
    for (int k = 0; k < nv; k++)
        if (assign[order[k]] == 0) { var = order[k]; break; }
    if (!var) { free(trail); return 1; }            /* all assigned, all sat */

    if (++decisions > cap) { capped = 1; }
    for (int val = 1; val >= -1 && !capped; val -= 2) {
        assign[var] = val;
        if (solve()) { free(trail); return 1; }
        assign[var] = 0;
    }
    while (ntrail) assign[abs(trail[--ntrail])] = 0;
    free(trail); return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) { fprintf(stderr, "usage: %s file.cnf [cap]\n", argv[0]);
                    return 1; }
    if (argc > 2) cap = atol(argv[2]);
    FILE *f = fopen(argv[1], "r");
    if (!f) { perror(argv[1]); return 1; }

    char line[1 << 16];
    int ci = 0;
    while (fgets(line, sizeof line, f)) {
        if (line[0] == 'c') continue;
        if (line[0] == 'p') {
            sscanf(line, "p cnf %d %d", &nv, &nc);
            cls = malloc(nc * sizeof(int *));
            clen = calloc(nc, sizeof(int));
            assign = calloc(nv + 1, sizeof(int));
            continue;
        }
        int buf[1 << 12], k = 0;
        char *p = line;
        for (;;) {                 /* one clause per line, 0-terminated */
            char *end;
            long lit = strtol(p, &end, 10);
            if (end == p || lit == 0) break;
            buf[k++] = (int)lit;
            p = end;
        }
        if (k) {
            cls[ci] = malloc(k * sizeof(int));
            memcpy(cls[ci], buf, k * sizeof(int));
            clen[ci++] = k;
        }
    }
    nc = ci;
    fclose(f);

    /* branching order: most frequent variables first */
    long *occ = calloc(nv + 1, sizeof(long));
    for (int c = 0; c < nc; c++)
        for (int k = 0; k < clen[c]; k++) occ[abs(cls[c][k])]++;
    order = malloc(nv * sizeof(int));
    for (int v = 0; v < nv; v++) order[v] = v + 1;
    for (int a = 0; a < nv; a++)            /* selection sort, nv is small */
        for (int b = a + 1; b < nv; b++)
            if (occ[order[b]] > occ[order[a]]) {
                int t = order[a]; order[a] = order[b]; order[b] = t;
            }

    int res = solve();
    if (capped) { printf("CAPPED after %ld decisions\n", decisions); return 30; }
    if (res) {
        printf("SAT after %ld decisions\nv", decisions);
        for (int v = 1; v <= nv; v++)
            printf(" %d", assign[v] == 1 ? v : -v);
        printf("\n");
        return 10;
    }
    printf("UNSAT after %ld decisions\n", decisions);
    return 20;
}
