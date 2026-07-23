#!/usr/bin/env python3
"""Exact algebraic stage: decide whether a coloring that survived P1/P2
actually admits complex weights making it a GHZ graph.

Input: an edge list with per-endpoint colors. Variables: one weight per
edge. System: for every realized mixed vertex-coloring class, the sum of
matching weights = 0; mono classes pairwise equal; Rabinowitsch t*mono_0
= 1 (forces the common value nonzero). Solvable over C iff the reduced
Groebner basis is not {1} (weak Nullstellensatz). If solvable, we
extract a solution and hand it to verifier.check_ghz for independent
confirmation — a verified solution IS a Krenn-Gu counterexample.
"""
import sympy as sp

from verifier import coloring_weights, check_ghz


def decide(n, ecols, d, verbose=True):
    """ecols: list of (u, v, cu, cv). Returns (status, witness_or_None).
    status in {"INFEASIBLE", "FEASIBLE"}."""
    ws = sp.symbols(f"w0:{len(ecols)}")
    t = sp.Symbol("t")
    edges = [(u, v, cu, cv, ws[i]) for i, (u, v, cu, cv) in enumerate(ecols)]
    W = coloring_weights(n, edges)
    mono = {c: W.get(tuple([c] * n), sp.Integer(0)) for c in range(d)}
    eqs = [sp.expand(v) for k, v in W.items() if len(set(k)) > 1]
    eqs += [sp.expand(mono[c] - mono[0]) for c in range(1, d)]
    eqs.append(sp.expand(t * mono[0] - 1))
    G = sp.groebner(eqs, *ws, t, order="grevlex")
    if list(G.exprs) == [sp.Integer(1)]:
        return "INFEASIBLE", None
    if verbose:
        print("  system is consistent — attempting explicit solution...")
    sols = sp.solve(eqs, list(ws) + [t], dict=True)
    for s in sols:
        vals = []
        for i in range(len(ecols)):
            expr = s.get(ws[i], ws[i])
            expr = expr.subs({sym: 1 for sym in expr.free_symbols})
            vals.append(sp.nsimplify(sp.simplify(expr)))
        cand = [(u, v, cu, cv, vals[i])
                for i, (u, v, cu, cv) in enumerate(ecols)]
        if all(w != 0 for *_, w in cand):
            ok, _ = check_ghz(n, cand, d, verbose=verbose)
            if ok:
                return "FEASIBLE", cand
    return "FEASIBLE", None            # consistent but no witness extracted


if __name__ == "__main__":
    one = 1
    print("[validation 1] hexagon d=2 (known GHZ) — expect FEASIBLE:")
    hexg = [(0, 1, 0, 0), (2, 3, 0, 0), (4, 5, 0, 0),
            (1, 2, 1, 1), (3, 4, 1, 1), (5, 0, 1, 1)]
    st, wit = decide(6, hexg, 2)
    print(f"  -> {st}")
    assert st == "FEASIBLE" and wit is not None

    print("[validation 2] naive K6 three-matching d=3 — expect INFEASIBLE:")
    naive = [(0, 1, 0, 0), (2, 3, 0, 0), (4, 5, 0, 0),
             (1, 2, 1, 1), (3, 4, 1, 1), (5, 0, 1, 1),
             (0, 3, 2, 2), (1, 4, 2, 2), (2, 5, 2, 2)]
    st, wit = decide(6, naive, 3)
    print(f"  -> {st}")
    assert st == "INFEASIBLE"

    print("[validation 3] K4 d=3 (known GHZ) — expect FEASIBLE:")
    k4 = [(0, 1, 0, 0), (2, 3, 0, 0), (0, 2, 1, 1),
          (1, 3, 1, 1), (0, 3, 2, 2), (1, 2, 2, 2)]
    st, wit = decide(4, k4, 3)
    print(f"  -> {st}")
    assert st == "FEASIBLE" and wit is not None
    print("algebraic stage validated on all three cases")
