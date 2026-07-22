#!/usr/bin/env python3
"""General CNF encoder for Ramsey instances.

Ramsey(s, t; n): variables are the edges of K_n (edge {i,j}, i<j, gets
DIMACS variable i*(2n-i-1)/2 + (j-i-1) + 1; True = red, False = blue).
Clauses forbid every red K_s (all-negative clause over the clique's edges)
and every blue K_t (all-positive clause).

The formula is satisfiable  iff  R(s,t) > n.
Proving it unsatisfiable proves  R(s,t) <= n.

--sym-break (only valid when s == t, where the color-swap symmetry exists):
  WLOG vertex 0's red edges form a prefix of its row (vertex relabeling), and
  vertex 0 has red-degree >= ceil((n-1)/2) (global color swap). With the
  prefix ordering, the degree constraint is the single unit clause x_{0,k}
  for k = ceil((n-1)/2). Both steps map any solution to a solution, so
  unsatisfiability of the constrained formula still proves R(s,s) <= n.
"""
import argparse
import math
import sys
from itertools import combinations


def var(n, i, j):
    if i > j:
        i, j = j, i
    return i * (2 * n - i - 1) // 2 + (j - i - 1) + 1


def clauses(s, t, n, sym_break=False):
    for cl in combinations(range(n), s):
        yield [-var(n, i, j) for i, j in combinations(cl, 2)]
    for cl in combinations(range(n), t):
        yield [var(n, i, j) for i, j in combinations(cl, 2)]
    if sym_break:
        assert s == t, "color-swap symmetry breaking requires s == t"
        for j in range(1, n - 1):          # x_{0,j+1} -> x_{0,j}
            yield [-var(n, 0, j + 1), var(n, 0, j)]
        yield [var(n, 0, math.ceil((n - 1) / 2))]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("s", type=int)
    ap.add_argument("t", type=int)
    ap.add_argument("n", type=int)
    ap.add_argument("--out", default=None)
    ap.add_argument("--sym-break", action="store_true")
    ap.add_argument("--stats-only", action="store_true")
    args = ap.parse_args()

    nvars = args.n * (args.n - 1) // 2
    ncl = sum(1 for _ in clauses(args.s, args.t, args.n, args.sym_break))
    print(f"Ramsey({args.s},{args.t};{args.n}): {nvars} variables, "
          f"{ncl} clauses", file=sys.stderr)
    if args.stats_only:
        return

    out = open(args.out, "w") if args.out else sys.stdout
    out.write(f"p cnf {nvars} {ncl}\n")
    for cl in clauses(args.s, args.t, args.n, args.sym_break):
        out.write(" ".join(map(str, cl)) + " 0\n")
    if args.out:
        out.close()


if __name__ == "__main__":
    main()
