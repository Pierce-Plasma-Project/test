#!/usr/bin/env python3
"""CDCL leg of the Ramsey pipeline, using CaDiCaL via python-sat.

Runs the upper-bound (UNSAT) instances and a witness search, and
independently re-verifies every SAT model against the clique constraints
(so a solver bug cannot silently produce a false witness).
"""
import time
from itertools import combinations

from pysat.solvers import Cadical195

from encode import clauses, var


def check_model(s, t, n, model):
    """Independently verify a model: no red K_s, no blue K_t."""
    red = {}
    for i, j in combinations(range(n), 2):
        red[(i, j)] = model[var(n, i, j) - 1] > 0
    for cl in combinations(range(n), s):
        if all(red[(i, j)] for i, j in combinations(cl, 2)):
            return False
    for cl in combinations(range(n), t):
        if not any(red[(i, j)] for i, j in combinations(cl, 2)):
            return False
    return True


def run(label, s, t, n, sym_break=False, conf_budget=None):
    cnf = list(clauses(s, t, n, sym_break))
    with Cadical195(bootstrap_with=cnf) as solver:
        t0 = time.perf_counter()
        if conf_budget:
            solver.conf_budget(conf_budget)
            res = solver.solve_limited()
        else:
            res = solver.solve()
        dt = time.perf_counter() - t0
        model = solver.get_model() if res else None

    if res is None:
        verdict = f"UNDECIDED within {conf_budget} conflicts"
    elif res:
        ok = check_model(s, t, n, model)
        verdict = f"SAT, model independently verified: {ok}"
    else:
        verdict = "UNSAT"
    print(f"{label:34s} {len(cnf):7d} clauses  {dt:8.2f}s  {verdict}",
          flush=True)
    return res


if __name__ == "__main__":
    print(f"solver: CaDiCaL 1.9.5 via python-sat\n")
    # sanity + the two computational leaves of the classical chain
    run("R(3,3;6)   upper-bound leaf", 3, 3, 6)          # expect UNSAT
    run("R(3,4;9)   upper-bound leaf", 3, 4, 9)          # expect UNSAT
    # witness search: solver rediscovers what Paley(17) gives by construction
    run("R(4,4;17)  witness search", 4, 4, 17)           # expect SAT
    # the theorem itself, direct: every 2-coloring of K18 has a mono K4
    run("R(4,4;18)  DIRECT, sym-broken", 4, 4, 18, sym_break=True)
    run("R(4,4;18)  DIRECT, raw", 4, 4, 18, conf_budget=20_000_000)
