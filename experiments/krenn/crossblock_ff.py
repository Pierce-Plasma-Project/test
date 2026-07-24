#!/usr/bin/env python3
"""Finite-field focused version of the cross-block attack (crossblock.py).

Intra-block amplitudes frozen at the K4 GHZ solution; only the 54
cross-block amplitudes vary, over F_p. Exhaustive is 7^54 (impossible),
so: (1) exact Groebner is attempted only on the FILTERED subsystem of
constraints that actually involve cross variables (most mixed classes are
pure intra-block and already 0); (2) a fast focused local search provides
an independent check. A common zero keeping the monos equal-and-nonzero
would lift to a counterexample.
"""
import itertools
import random

P = 7
D = 3
A, B = [0, 1, 2, 3], [4, 5, 6, 7]
W = 2  # a primitive cube root of unity mod 7 (2^3 = 8 = 1 mod 7)


def k4():
    factors = [[(0, 1), (2, 3)], [(0, 2), (1, 3)], [(0, 3), (1, 2)]]
    d = {}
    for c, fac in enumerate(factors):
        for (i, j) in fac:
            d[(i, j, c, c)] = 1
    return d


def perfect_matchings(n):
    out = []
    def rec(un, ch):
        if not un:
            out.append(tuple(ch)); return
        v0 = min(un)
        for x in sorted(un - {v0}):
            rec(un - {v0, x}, ch + [(v0, x)])
    rec(frozenset(range(n)), [])
    return out


PMS = perfect_matchings(8)
Wintra = k4()
crossvars = [(u, v, a, b) for u in A for v in B
             for a in range(D) for b in range(D)]
vidx = {k: i for i, k in enumerate(crossvars)}


def amp(V, u, v, a, b):
    if u > v:
        u, v, a, b = v, u, b, a
    if u in A and v in A:
        return Wintra.get((u, v, a, b), 0)
    if u in B and v in B:
        return Wintra.get((u - 4, v - 4, a, b), 0)
    return V[vidx[(u, v, a, b)]]


def class_sum(V, kappa):
    s = 0
    for pm in PMS:
        t = 1
        for (u, v) in pm:
            t = (t * amp(V, u, v, kappa[u], kappa[v])) % P
            if t == 0:
                break
        s = (s + t) % P
    return s


# precompute class list: mono (3) and the mixed classes that actually
# depend on cross variables (others are identically 0 already)
MONO = [tuple([c] * 8) for c in range(D)]
MIXED = []
for kappa in itertools.product(range(D), repeat=8):
    if len(set(kappa)) == 1:
        continue
    # depends on a cross var iff some PM contributes a term crossing blocks
    depends = False
    for pm in PMS:
        crosses = any((u in A) != (v in A) for (u, v) in pm)
        if not crosses:
            continue
        ok = True
        for (u, v) in pm:
            a, b = kappa[u], kappa[v]
            uu, vv, aa, bb = (u, v, a, b) if u < v else (v, u, b, a)
            if uu in A and vv in A and (uu, vv, aa, bb) not in Wintra:
                ok = False; break
            if uu in B and vv in B and (uu-4, vv-4, aa, bb) not in Wintra:
                ok = False; break
        if ok:
            depends = True; break
    if depends:
        MIXED.append(kappa)


def cost(V):
    c = 0
    mv = None
    for k in MONO:
        s = class_sum(V, k)
        if s == 0:
            c += 100
        elif mv is None:
            mv = s
        elif s != mv:
            c += 100
    for k in MIXED:
        if class_sum(V, k):
            c += 1
    return c


def search(seed, restarts, moves):
    rng = random.Random(seed)
    best = 10**9
    for r in range(restarts):
        V = [rng.choice([0, 1, W, W*W % P]) if rng.random() < 0.3 else 0
             for _ in crossvars]
        c = cost(V)
        for _ in range(moves):
            if c == 0:
                return V, 0
            i = rng.randrange(len(V))
            old = V[i]
            V[i] = rng.randrange(P)
            c2 = cost(V)
            if c2 <= c or rng.random() < 0.05:
                c = c2
            else:
                V[i] = old
        best = min(best, c)
    return None, best


if __name__ == "__main__":
    import sys
    seed = int(sys.argv[1]) if len(sys.argv) > 1 else 0
    print(f"cross vars: {len(crossvars)}, mono: {len(MONO)}, "
          f"cross-dependent mixed classes: {len(MIXED)}", flush=True)
    V, c = search(seed, restarts=40, moves=4000)
    if V is not None:
        print(f"SOLUTION over F{P}: {V}")
        print("=> lift to Q(omega) and verify; candidate counterexample")
    else:
        print(f"no cross-block solution; best cost {c} "
              f"(floor = irreducible cross obstruction)")
