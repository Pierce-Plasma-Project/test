/*
 * Random 3-SAT phase transition experiment.
 *
 * For each clause/variable ratio r, generate T random 3-SAT instances with
 * n variables and m = round(r * n) clauses (three distinct variables per
 * clause, random polarities), solve each with a plain DPLL procedure
 * (unit propagation + chronological backtracking), and record satisfiability
 * and the number of branching decisions.
 *
 * Expected (and well-established) picture: the satisfiable fraction crosses
 * 1/2 near r ~ 4.27, and search cost peaks sharply in the same window.
 * Fixed RNG seed => the run is exactly reproducible.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define NVARS 60
#define MAXCL 400
#define TRIALS 100
#define DECISION_CAP 2000000L

static int cl[MAXCL][3];
static int m;
static int assign[NVARS + 1];   /* 0 = unset, 1 = true, -1 = false */
static long decisions;
static int giveup;

static uint64_t rng_state = 0x9E3779B97F4A7C15ULL;  /* fixed seed */
static uint64_t xorshift(void)
{
    uint64_t x = rng_state;
    x ^= x << 13; x ^= x >> 7; x ^= x << 17;
    return rng_state = x;
}
static int rnd(int k) { return (int)(xorshift() % (uint64_t)k); }

static void gen(int n, int mm)
{
    m = mm;
    for (int c = 0; c < m; c++) {
        int v[3];
        v[0] = rnd(n) + 1;
        do { v[1] = rnd(n) + 1; } while (v[1] == v[0]);
        do { v[2] = rnd(n) + 1; } while (v[2] == v[0] || v[2] == v[1]);
        for (int k = 0; k < 3; k++)
            cl[c][k] = rnd(2) ? v[k] : -v[k];
    }
}

static int lit_val(int lit)
{
    int a = assign[abs(lit)];
    if (a == 0) return 0;
    return (lit > 0) ? a : -a;
}

/* returns 1 SAT, -1 conflict-free but needs branching done inside, 0 UNSAT */
static int solve(void)
{
    if (giveup) return 0;

    /* unit propagation; remember trail to undo */
    int trail[NVARS], ntrail = 0;
    for (;;) {
        int changed = 0;
        for (int c = 0; c < m; c++) {
            int unset = 0, sat = 0, last = 0;
            for (int k = 0; k < 3; k++) {
                int v = lit_val(cl[c][k]);
                if (v == 1) { sat = 1; break; }
                if (v == 0) { unset++; last = cl[c][k]; }
            }
            if (sat) continue;
            if (unset == 0) {            /* conflict */
                while (ntrail) assign[trail[--ntrail]] = 0;
                return 0;
            }
            if (unset == 1) {            /* unit clause */
                assign[abs(last)] = last > 0 ? 1 : -1;
                trail[ntrail++] = abs(last);
                changed = 1;
            }
        }
        if (!changed) break;
    }

    int var = 0;
    for (int v = 1; v <= NVARS; v++)
        if (assign[v] == 0) { var = v; break; }
    if (!var) {                          /* full assignment, all clauses sat */
        while (ntrail) assign[trail[--ntrail]] = 0;
        return 1;
    }

    if (++decisions > DECISION_CAP) { giveup = 1; }

    for (int val = 1; val >= -1; val -= 2) {
        assign[var] = val;
        if (solve()) { assign[var] = 0;
                       while (ntrail) assign[trail[--ntrail]] = 0;
                       return 1; }
        assign[var] = 0;
        if (giveup) break;
    }
    while (ntrail) assign[trail[--ntrail]] = 0;
    return 0;
}

static int cmplong(const void *a, const void *b)
{
    long x = *(const long *)a, y = *(const long *)b;
    return (x > y) - (x < y);
}

int main(void)
{
    double ratios[] = {3.0, 3.4, 3.8, 4.0, 4.1, 4.2, 4.27, 4.35,
                       4.5, 4.8, 5.2, 5.6, 6.0};
    int nr = (int)(sizeof ratios / sizeof ratios[0]);

    printf("r,n,trials,sat_fraction,median_decisions,p90_decisions,"
           "max_decisions,capped_runs\n");
    for (int ri = 0; ri < nr; ri++) {
        int mm = (int)(ratios[ri] * NVARS + 0.5);
        long dec[TRIALS];
        int nsat = 0, ncap = 0;
        for (int t = 0; t < TRIALS; t++) {
            gen(NVARS, mm);
            memset(assign, 0, sizeof assign);
            decisions = 0; giveup = 0;
            int s = solve();
            if (giveup) { ncap++; dec[t] = DECISION_CAP; }
            else        { dec[t] = decisions; if (s) nsat++; }
        }
        qsort(dec, TRIALS, sizeof(long), cmplong);
        printf("%.2f,%d,%d,%.2f,%ld,%ld,%ld,%d\n",
               ratios[ri], NVARS, TRIALS,
               (double)nsat / TRIALS,
               dec[TRIALS / 2], dec[(TRIALS * 9) / 10], dec[TRIALS - 1],
               ncap);
        fflush(stdout);
    }
    return 0;
}
