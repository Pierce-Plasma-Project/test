#!/usr/bin/env python3
"""Symmetry-ansatz hunter for Krenn-Gu counterexamples, full generality.

The complete quantum formulation: every vertex pair {u,v} of K_n carries
a d x d amplitude matrix W_uv (this subsumes all multigraphs and all
bichromatic colorings simultaneously: a parallel edge colored (a,b) is
just a nonzero entry W_uv[a][b]). For a vertex coloring kappa, its
weight is  sum over perfect matchings M of  prod_{(u,v) in M}
W_uv[kappa(u)][kappa(v)].  A GHZ graph of dimension d exists on n
vertices iff the system {mixed kappa -> 0, mono classes equal nonzero}
is solvable.

Ansatz layer: a map (pair, a, b) -> variable index, shrinking the
variable count so exhaustive search over a small finite field F_p is
feasible. Any F_p solution is a candidate for lifting to C (choose p
with the right roots of unity, e.g. p=7 for cube roots when d=3).

This module emits the monomial tables consumed by fsearch.c:
  line 1: NV NCLASS NPM n d
  then per class: "M c" (1 mono of color c / 0 mixed) then NPM monomials,
  each = n/2 variable indices.
Classes are pruned: a class whose PM list is empty is identically zero
and imposes no condition (mixed) or kills the ansatz (mono, handled by
the searcher seeing an empty mono class marker -1).
"""
import itertools
import sys


def pms_of_Kn(n):
    out = []
    def rec(un, ch):
        if not un:
            out.append(list(ch)); return
        v0 = min(un)
        for w in sorted(un - {v0}):
            rec(un - {v0, w}, ch + [(v0, w)])
    rec(frozenset(range(n)), [])
    return out


def ansatz_circulant_colorcyclic(n, d):
    """W_{uv}[a][b] depends only on (dist(u,v), (b-a) mod d) where
    dist = min(|u-v|, n-|u-v|). Variables: ndist * d."""
    ndist = n // 2
    nv = ndist * d
    def m(u, v, a, b):
        dd = min(abs(u - v), n - abs(u - v)) - 1
        return dd * d + ((b - a) % d)
    return nv, m, "circ-colorcyclic"


def ansatz_circulant_full(n, d):
    """W depends on (dist, a, b): ndist * d * d variables."""
    ndist = n // 2
    nv = ndist * d * d
    def m(u, v, a, b):
        dd = min(abs(u - v), n - abs(u - v)) - 1
        return dd * d * d + a * d + b
    return nv, m, "circ-full"


def ansatz_dist_diag_offdiag(n, d):
    """W[a][b] depends on (dist, a==b): 2 * ndist variables — the
    crudest ansatz, for fast negative scans."""
    ndist = n // 2
    nv = ndist * 2
    def m(u, v, a, b):
        dd = min(abs(u - v), n - abs(u - v)) - 1
        return dd * 2 + (0 if a == b else 1)
    return nv, m, "dist-diag"


def ansatz_pair_full(n, d):
    """One variable per (pair, a, b): the completely general system."""
    pairs = {}
    for u in range(n):
        for v in range(u + 1, n):
            pairs[(u, v)] = len(pairs)
    nv = len(pairs) * d * d
    def m(u, v, a, b):
        if u > v:
            u, v, a, b = v, u, b, a
        return pairs[(u, v)] * d * d + a * d + b
    return nv, m, "pair-full"


ANSATZE = {
    "cc": ansatz_circulant_colorcyclic,
    "cf": ansatz_circulant_full,
    "dd": ansatz_dist_diag_offdiag,
    "pf": ansatz_pair_full,
}


def emit(n, d, akey, out=sys.stdout):
    nv, m, name = ANSATZE[akey](n, d)
    pms = pms_of_Kn(n)
    classes = []
    for kappa in itertools.product(range(d), repeat=n):
        mono = len(set(kappa)) == 1
        mons = []
        for pm in pms:
            mon = sorted(m(u, v, kappa[u], kappa[v]) for (u, v) in pm)
            mons.append(mon)
        classes.append((1 if mono else 0, kappa[0] if mono else -1, mons))
    print(f"ansatz {name}: n={n} d={d} vars={nv} classes={len(classes)} "
          f"pms={len(pms)}", file=sys.stderr)
    print(nv, len(classes), len(pms), n, d, file=out)
    for mono, c, mons in classes:
        print(mono, c, file=out)
        for mon in mons:
            print(" ".join(map(str, mon)), file=out)


if __name__ == "__main__":
    n, d, akey = int(sys.argv[1]), int(sys.argv[2]), sys.argv[3]
    emit(n, d, akey)
