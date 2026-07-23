#!/usr/bin/env python3
"""Exact verifier for candidate counterexamples to the Krenn-Gu conjecture.

Setup (reconstructed from the literature; cross-check against
mariokrenn.wordpress.com/graph-theory-question/ and arXiv:2407.00303 —
both unreachable from this environment's network):

A candidate is a multigraph on vertices 0..n-1. Each edge e = (u, v)
carries a weight w_e (exact: Gaussian rationals via sympy) and an
ordered color pair (c_u, c_v): the color e shows at u, and at v
(bichromatic edges allowed; monochromatic means c_u = c_v).

A perfect matching M induces the vertex coloring that gives each vertex
the color shown by its matching edge. Weight of M = product of edge
weights. For a vertex coloring kappa, w(kappa) = sum of weights of all
perfect matchings inducing kappa.

G is a GHZ graph of dimension d if, for some nonzero constant s:
  * w(all-vertices-colored-c) = s for each of the d colors c, and
  * w(kappa) = 0 for every non-monochromatic kappa.

Krenn-Gu conjecture: n > 4 forces d <= 2. A verified candidate with
n >= 6, d >= 3 is a counterexample (and wins Krenn's bounty).
"""
import itertools
from collections import defaultdict

import sympy as sp


def perfect_matchings(n, edges):
    """Yield perfect matchings as tuples of edge indices. edges[i] =
    (u, v, cu, cv, w). Multigraph-safe."""
    def rec(unmatched, chosen, start):
        if not unmatched:
            yield tuple(chosen)
            return
        v0 = min(unmatched)
        for i in range(len(edges)):
            u, v, *_ = edges[i]
            if u == v0 and v in unmatched and v != v0:
                yield from rec(unmatched - {u, v}, chosen + [i], 0)
            elif v == v0 and u in unmatched and u != v0:
                yield from rec(unmatched - {u, v}, chosen + [i], 0)
    yield from rec(frozenset(range(n)), [], 0)


def coloring_weights(n, edges):
    """Map induced vertex coloring -> exact total weight."""
    sums = defaultdict(lambda: sp.Integer(0))
    for M in perfect_matchings(n, edges):
        kappa = [None] * n
        w = sp.Integer(1)
        for i in M:
            u, v, cu, cv, wt = edges[i]
            kappa[u], kappa[v] = cu, cv
            w *= wt
        sums[tuple(kappa)] += w
    return {k: sp.simplify(v) for k, v in sums.items()}


def check_ghz(n, edges, d, verbose=True):
    """Return (is_ghz, report). Requires every mono coloring to have one
    common nonzero weight and every mixed coloring weight to vanish."""
    W = coloring_weights(n, edges)
    mono = {c: W.get(tuple([c] * n), sp.Integer(0)) for c in range(d)}
    bad = {k: v for k, v in W.items()
           if len(set(k)) > 1 and sp.simplify(v) != 0}
    s = mono[0]
    ok = (s != 0 and all(sp.simplify(mono[c] - s) == 0 for c in range(d))
          and not bad)
    if verbose:
        for c in range(d):
            print(f"    mono color {c}: weight {mono[c]}")
        print(f"    nonzero mixed colorings: {len(bad)}")
        for k, v in list(bad.items())[:5]:
            print(f"      {k}: {v}")
        print(f"    GHZ dimension {d}: {'YES' if ok else 'no'}")
    return ok, (mono, bad)


if __name__ == "__main__":
    one = sp.Integer(1)

    print("[sanity 1] K4 with three monochromatic perfect matchings")
    print("           (the known n=4, d=3 GHZ graph — why n>4 is required):")
    k4 = [(0, 1, 0, 0, one), (2, 3, 0, 0, one),
          (0, 2, 1, 1, one), (1, 3, 1, 1, one),
          (0, 3, 2, 2, one), (1, 2, 2, 2, one)]
    ok, _ = check_ghz(4, k4, 3)
    assert ok

    print("[sanity 2] hexagon with alternating colors (n=6, d=2):")
    hexg = [(0, 1, 0, 0, one), (2, 3, 0, 0, one), (4, 5, 0, 0, one),
            (1, 2, 1, 1, one), (3, 4, 1, 1, one), (5, 0, 1, 1, one)]
    ok, _ = check_ghz(6, hexg, 2)
    assert ok

    print("[sanity 3] naive K6 analogue of sanity 1 (three edge-disjoint")
    print("           mono matchings on 6 vertices) — must FAIL, showing")
    print("           the cross-term obstruction the conjecture is about:")
    naive = [(0, 1, 0, 0, one), (2, 3, 0, 0, one), (4, 5, 0, 0, one),
             (1, 2, 1, 1, one), (3, 4, 1, 1, one), (5, 0, 1, 1, one),
             (0, 3, 2, 2, one), (1, 4, 2, 2, one), (2, 5, 2, 2, one)]
    ok, (mono, bad) = check_ghz(6, naive, 3)
    assert not ok
    print("\nverifier behaves correctly on all three sanity cases")
