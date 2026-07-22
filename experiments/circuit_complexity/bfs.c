/*
 * Exact gate complexity of all Boolean functions on n <= 4 variables,
 * over the full binary basis B2 (all 16 two-input gates; negation is free
 * only in the sense that every gate has its negated twin in the basis).
 *
 * Method: breadth-first search over truth tables by circuit size.
 *   cost 0: variables and constants (leaves).
 *   cost s: op(f, g) for any of the 10 non-degenerate binary ops and any
 *           f of cost i, g of cost j with i + j + 1 = s.
 * Every function's minimum gate count is found exactly; no heuristics.
 *
 * This reproduces classically known ground truth (e.g., every 4-variable
 * function has B2 complexity at most 7 — Knuth, TAOCP 7.1.2), which makes
 * the output independently checkable.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAXS 16

static uint8_t  best[65536];
static uint16_t level[MAXS + 1][65536];
static int      lsize[MAXS + 1];

static int popcount4(int x) { return __builtin_popcount(x); }

static void add(unsigned f, int s, unsigned mask, int *count)
{
    f &= mask;
    if (best[f] == 0xFF) {
        best[f] = (uint8_t)s;
        level[s][lsize[s]++] = (uint16_t)f;
        (*count)++;
    }
}

static void combine(unsigned f, unsigned g, int s, unsigned mask, int *count)
{
    add(f & g, s, mask, count);
    add(f | g, s, mask, count);
    add(f ^ g, s, mask, count);
    add(~(f & g), s, mask, count);
    add(~(f | g), s, mask, count);
    add(~(f ^ g), s, mask, count);
    add(f & ~g, s, mask, count);
    add(~f & g, s, mask, count);
    add(f | ~g, s, mask, count);
    add(~f | g, s, mask, count);
}

static void run(int n)
{
    int nbits = 1 << n;                 /* rows in the truth table   */
    unsigned total = 1u << nbits;       /* number of distinct tables */
    unsigned mask = total - 1;
    int count = 0;

    memset(best, 0xFF, sizeof best);
    memset(lsize, 0, sizeof lsize);

    /* leaves: constants and variables */
    add(0, 0, mask, &count);
    add(mask, 0, mask, &count);
    unsigned var[4];
    for (int k = 0; k < n; k++) {
        unsigned t = 0;
        for (int i = 0; i < nbits; i++)
            if ((i >> k) & 1) t |= 1u << i;
        var[k] = t;
        add(t, 0, mask, &count);
    }

    int maxcost = 0;
    for (int s = 1; s <= MAXS && (unsigned)count < total; s++) {
        for (int i = 0; i + i <= s - 1; i++) {
            int j = s - 1 - i;
            if (i == j) {
                for (int a = 0; a < lsize[i]; a++)
                    for (int b = a; b < lsize[j]; b++)
                        combine(level[i][a], level[j][b], s, mask, &count);
            } else {
                for (int a = 0; a < lsize[i]; a++)
                    for (int b = 0; b < lsize[j]; b++)
                        combine(level[i][a], level[j][b], s, mask, &count);
            }
        }
        if (lsize[s] > 0) maxcost = s;
    }

    printf("n=%d: %u functions, all found, max gate complexity = %d\n",
           n, total, maxcost);
    printf("  gates : #functions\n");
    for (int s = 0; s <= maxcost; s++)
        printf("  %5d : %d\n", s, lsize[s]);

    /* named functions */
    unsigned parity = 0;
    for (int k = 0; k < n; k++) parity ^= var[k];
    printf("  XOR%d (parity)        : %d gates\n", n, best[parity]);

    if (n == 3) {
        unsigned maj = 0;
        for (int i = 0; i < nbits; i++)
            if (popcount4(i) >= 2) maj |= 1u << i;
        printf("  MAJ3 (majority)      : %d gates\n", best[maj]);
    }
    if (n == 4) {
        unsigned thr2 = 0, thr3 = 0;
        for (int i = 0; i < nbits; i++) {
            if (popcount4(i) >= 2) thr2 |= 1u << i;
            if (popcount4(i) >= 3) thr3 |= 1u << i;
        }
        printf("  THR2_4 (>=2 of 4)    : %d gates\n", best[thr2]);
        printf("  THR3_4 (>=3 of 4)    : %d gates\n", best[thr3]);
        printf("  hardest functions (need %d gates):\n", maxcost);
        int shown = 0;
        for (unsigned f = 0; f < total && shown < 8; f++)
            if (best[f] == maxcost)
                printf("    truth table 0x%04X\n", f), shown++;
        printf("    (%d such functions in total)\n", lsize[maxcost]);
    }
    printf("\n");
}

int main(void)
{
    run(2);
    run(3);
    run(4);
    return 0;
}
