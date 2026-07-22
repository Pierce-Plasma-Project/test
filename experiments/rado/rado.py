#!/usr/bin/env python3
"""Three-color Rado numbers via SAT, with verified certificates.

R_3(E) for a linear equation E: the smallest n such that every 3-coloring of
{1..n} contains a monochromatic solution of E. Solutions allow repeated
values (the standard convention; calibrated against R_3(x+y=z) = 14 and the
proven family R_3(mx+y=z) = (m+2)^3-(m+2)^2-(m+2)-1, Chang-De Loera-Wesley
2022 / Myers).

Encoding (direct): variables v(i,c) "integer i has color c", c in {0,1,2}.
  - at-least-one and at-most-one color per i;
  - for every solution (x,y,z) of E in [1,n]^3 and every color c, a clause
    forbidding x,y,z from all having color c (collapsed when x,y,z repeat).
R_3(E) = n  iff  instance(n-1) is SAT and instance(n) is UNSAT.

Clauses are STREAMED into the solver's internal (C) storage — no Python
clause list is ever materialized, so instance size is bounded by solver
memory, not Python object overhead. Boundary cells run to ~10^7-10^8
clauses this way.

Pipeline per computed value:
  1. exponential + binary search for the SAT/UNSAT boundary with CaDiCaL;
  2. every SAT witness is re-verified by an independent checker;
  3. the UNSAT boundary instance is re-solved by Glucose 3 with DRUP proof
     logging (a second, independent solver); the proof and CNF are written
     to results/rado/ and checked with drat-trim when available.

Equation input: coefficients (a, b, c) meaning  a*x + b*y = c*z.

Caution for equations with no zero-sum coefficient subset {a, b, -c}: by
Rado's theorem such equations are not partition regular and R_3 may be
infinite — SAT search alone can never establish that (cf. the symbolic-set
method, arXiv:2505.12085). MAX_N aborts runaway searches.
"""
import argparse
import os
import subprocess
import sys
import time

from pysat.solvers import Cadical195, Glucose3

RESULTS = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                       "..", "..", "results", "rado")

MAX_CLAUSES = 90_000_000
MAX_N = 250_000


def solutions(a, b, c, n):
    """Yield all (x,y,z) in [1,n]^3 with a*x + b*y = c*z."""
    cn = c * n
    for x in range(1, n + 1):
        ax = a * x
        if ax + b > cn:
            break
        for y in range(1, n + 1):
            v = ax + b * y
            if v > cn:
                break
            if v % c == 0:
                yield (x, y, v // c)


def var(i, col):
    return 3 * (i - 1) + col + 1          # DIMACS, 1-based


def clause_stream(a, b, c, n):
    for i in range(1, n + 1):
        yield [var(i, 0), var(i, 1), var(i, 2)]
        yield [-var(i, 0), -var(i, 1)]
        yield [-var(i, 0), -var(i, 2)]
        yield [-var(i, 1), -var(i, 2)]
    for (x, y, z) in solutions(a, b, c, n):
        for col in range(3):
            yield sorted({-var(x, col), -var(y, col), -var(z, col)})


def check_witness(a, b, c, n, model):
    """Independent check: model is a proper 3-coloring of [1,n] with no
    monochromatic solution. Shares no code with the encoder's clause loop."""
    color = {}
    for i in range(1, n + 1):
        cols = [col for col in range(3) if model[var(i, col) - 1] > 0]
        if len(cols) != 1:
            return False, None
        color[i] = cols[0]
    for (x, y, z) in solutions(a, b, c, n):
        if color[x] == color[y] == color[z]:
            return False, None
    return True, color


def solve(a, b, c, n, progress=False):
    if n > MAX_N:
        raise RuntimeError(
            f"n={n} exceeds MAX_N; if the equation is not partition "
            f"regular, R_3 may be infinite (SAT search cannot prove that)")
    t0 = time.perf_counter()
    ncl = 0
    with Cadical195() as s:
        for cl in clause_stream(a, b, c, n):
            s.add_clause(cl)
            ncl += 1
            if ncl > MAX_CLAUSES:
                raise RuntimeError(f"clause cap exceeded at n={n}")
        tload = time.perf_counter() - t0
        res = s.solve()
        model = s.get_model() if res else None
    if progress:
        print(f"    n={n}: {ncl} clauses, load {tload:.0f}s, "
              f"solve {time.perf_counter()-t0-tload:.0f}s -> "
              f"{'SAT' if res else 'UNSAT'}", flush=True)
    return res, model


def certify_unsat(a, b, c, n, tag):
    """Re-solve the UNSAT boundary with Glucose3 (independent of CaDiCaL),
    logging a DRUP proof; write CNF + proof; verify with drat-trim."""
    os.makedirs(RESULTS, exist_ok=True)
    cnfpath = os.path.join(RESULTS, f"{tag}.cnf")
    proofpath = os.path.join(RESULTS, f"{tag}.drat")
    ncl = sum(1 for _ in clause_stream(a, b, c, n))
    t0 = time.perf_counter()
    with Glucose3(with_proof=True) as s, open(cnfpath, "w") as f:
        f.write(f"p cnf {3*n} {ncl}\n")
        for cl in clause_stream(a, b, c, n):
            s.add_clause(cl)
            f.write(" ".join(map(str, cl)) + " 0\n")
        assert s.solve() is False, "certify_unsat: Glucose disagrees (SAT)"
        proof = s.get_proof()
    with open(proofpath, "w") as f:
        f.write("\n".join(proof) + "\n")
    dt = time.perf_counter() - t0
    checker = os.environ.get("DRAT_TRIM", "drat-trim")
    try:
        out = subprocess.run([checker, cnfpath, proofpath],
                             capture_output=True, text=True, timeout=7200)
        status = ("VERIFIED" if "s VERIFIED" in out.stdout else
                  "NOT-VERIFIED")
    except (FileNotFoundError, subprocess.TimeoutExpired):
        status = "checker-unavailable"
    return proofpath, status, dt


def rado3(a, b, c, start=2, verbose=True):
    """Compute R_3(ax+by=cz) by exponential + binary search."""
    t0 = time.perf_counter()
    n = max(start, 2)
    last_sat = 0
    while True:                                   # exponential phase
        res, model = solve(a, b, c, n, progress=verbose)
        if res:
            ok, _ = check_witness(a, b, c, n, model)
            assert ok, f"witness check FAILED at n={n}"
            last_sat, n = n, n * 2
        else:
            break
    lo, hi = last_sat, n
    while hi - lo > 1:                            # binary phase
        mid = (lo + hi) // 2
        res, model = solve(a, b, c, mid, progress=verbose)
        if res:
            ok, _ = check_witness(a, b, c, mid, model)
            assert ok, f"witness check FAILED at n={mid}"
            lo = mid
        else:
            hi = mid
    res_low, model = solve(a, b, c, hi - 1)       # confirm boundary
    assert res_low, "boundary inconsistency: n-1 not SAT"
    ok, coloring = check_witness(a, b, c, hi - 1, model)
    assert ok, "final witness check FAILED"
    res_high, _ = solve(a, b, c, hi)
    assert res_high is False, "boundary inconsistency: n not UNSAT"
    if verbose:
        print(f"R_3({a}x+{b}y={c}z) = {hi}   "
              f"[witness at {hi-1} verified; "
              f"{time.perf_counter()-t0:.1f}s total]", flush=True)
    return hi, coloring


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("a", type=int)
    ap.add_argument("b", type=int)
    ap.add_argument("c", type=int)
    ap.add_argument("--start", type=int, default=2,
                    help="initial n for the exponential phase")
    ap.add_argument("--certify", action="store_true")
    ap.add_argument("--witness", action="store_true")
    args = ap.parse_args()
    r, coloring = rado3(args.a, args.b, args.c, start=args.start)
    if args.witness:
        parts = {0: [], 1: [], 2: []}
        for i, col in sorted(coloring.items()):
            parts[col].append(i)
        for col in range(3):
            print(f"  color {col}: {parts[col]}")
    if args.certify:
        tag = f"r3_{args.a}_{args.b}_{args.c}_n{r}"
        path, status, dt = certify_unsat(args.a, args.b, args.c, r, tag)
        print(f"  UNSAT certificate: {path}  [{status}, {dt:.0f}s]",
              flush=True)


if __name__ == "__main__":
    main()
