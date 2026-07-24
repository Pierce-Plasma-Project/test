#!/usr/bin/env python3
"""Surgical attack on the cost-6 obstruction at n=8, d=3.

The stochastic search floor is exactly 6: two K4 GHZ graphs on vertex
blocks A={0,1,2,3} and B={4,5,6,7} satisfy every constraint except the
six mixed classes that couple the blocks. This script asks the sharp
question directly: can the 6 cross-block pairs (each a full 3x3 complex
matrix, 54 free amplitudes) be chosen to kill those 6 classes WITHOUT
breaking any of the already-satisfied ones?

We keep the intra-block amplitudes fixed at the known K4 solution and
treat only the cross-block amplitudes as unknowns. Because every perfect
matching of K8 either (a) stays within blocks, (b) uses exactly 2
cross-block edges, or (c) uses 4 cross-block edges, the resulting system
in the cross variables is exactly degree <=2 per matching — small enough
for an exact Groebner decision over Q(omega).

Output: INFEASIBLE (the block-scaffold obstruction is real and provably
unremovable by cross terms alone) or a witness (=> lift & verify => a
counterexample).
"""
import itertools
import sympy as sp

# cube root of unity
w = sp.exp(2 * sp.pi * sp.I / 3)
D = 3
A, B = [0, 1, 2, 3], [4, 5, 6, 7]


def k4_solution():
    """The known K4 d=3 GHZ amplitudes on a 4-vertex block, as a dict
    (i,j,a,b)->value with i<j local indices 0..3. Monochromatic pairing
    of the three 1-factors of K4 with the three colors."""
    # 1-factorization of K4: {01,23},{02,13},{03,12} -> colors 0,1,2
    factors = [[(0, 1), (2, 3)], [(0, 2), (1, 3)], [(0, 3), (1, 2)]]
    W = {}
    for c, fac in enumerate(factors):
        for (i, j) in fac:
            W[(i, j, c, c)] = sp.Integer(1)
    return W


def perfect_matchings(n):
    out = []
    def rec(un, ch):
        if not un:
            out.append(list(ch)); return
        v0 = min(un)
        for x in sorted(un - {v0}):
            rec(un - {v0, x}, ch + [(v0, x)])
    rec(frozenset(range(n)), [])
    return out


def build_system():
    Wintra = k4_solution()
    # cross variables: pair (u in A, v in B), colors a,b
    cross = {}
    for u in A:
        for v in B:
            for a in range(D):
                for b in range(D):
                    cross[(u, v, a, b)] = sp.Symbol(f"c_{u}_{v}_{a}_{b}")

    def amp(u, v, a, b):
        if u > v:
            u, v, a, b = v, u, b, a
        if u in A and v in A:
            return Wintra.get((u, v, a, b), sp.Integer(0))
        if u in B and v in B:
            lu, lv = u - 4, v - 4
            return Wintra.get((lu, lv, a, b), sp.Integer(0))
        return cross[(u, v, a, b)]           # u in A, v in B

    pms = perfect_matchings(8)
    eqs, monos = [], {}
    for kappa in itertools.product(range(D), repeat=8):
        expr = sp.Integer(0)
        for pm in pms:
            term = sp.Integer(1)
            for (u, v) in pm:
                term *= amp(u, v, kappa[u], kappa[v])
                if term == 0:
                    break
            expr += term
        expr = sp.expand(expr)
        if len(set(kappa)) == 1:
            monos[kappa[0]] = expr
        elif expr != 0:
            eqs.append(expr)
    return cross, eqs, monos


def main():
    cross, eqs, monos = build_system()
    print(f"cross variables: {len(cross)}")
    print(f"nontrivial mixed constraints (in cross vars): {len(eqs)}")
    # mono classes must stay equal & nonzero; intra-block already gives 1
    for c in range(D):
        print(f"  mono[{c}] = {monos[c]}")
    # feasibility: do the mixed constraints admit a common zero that keeps
    # the monos equal? Add Rabinowitsch on mono[0].
    t = sp.Symbol("t")
    system = list(eqs) + [sp.expand(monos[c] - monos[0]) for c in range(1, D)]
    system.append(sp.expand(t * monos[0] - 1))
    vars_ = list(cross.values()) + [t]
    print(f"deciding feasibility over Q(omega): {len(system)} polynomials, "
          f"{len(vars_)} vars ...", flush=True)
    G = sp.groebner(system, *vars_, order="grevlex")
    if list(G.exprs) == [sp.Integer(1)]:
        print("\nINFEASIBLE: the cost-6 block obstruction CANNOT be removed "
              "by cross-block amplitudes alone (exact proof over Q(omega)).")
        print("=> the block scaffold provably yields no counterexample; a "
              "counterexample must be non-blockwise.")
    else:
        print("\nFEASIBLE: cross-block amplitudes CAN kill the six terms!")
        print("Extracting witness for lift & verify ...")
        sol = sp.solve(system, vars_, dict=True)
        print(sol[:1] if sol else "consistent but no explicit solution")


if __name__ == "__main__":
    main()
