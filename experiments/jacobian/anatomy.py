#!/usr/bin/env python3
"""Computational anatomy of the Alpoge/Fable 5 Jacobian counterexample.

F = (P,Q,R) : C^3 -> C^3,  det JF = -2 identically (verified in verify.py).

Structure exploited: F is AFFINE IN z,  F = A(x,y) z + B(x,y), with
  A = ((1+xy)^3, 3x(1+xy)^2, -x^3),
  B = (y^2(1+xy)(4+3xy),  y + 3xy^2(4+3xy),  2x - 3x^2 y).

Fibers over a target (p,q,r) stratify:
  * x = 0: F(0,y,z) = (z + 4y^2, y, 0) — the plane {x=0} maps bijectively
    onto the plane {r=0}. Preimage (0, q, p - 4q^2), present iff r = 0.
  * x != 0: A3 = -x^3 != 0, so z = (r - B3)/A3, and (x,y) solves
        H1 = A1 (r - B3) - A3 (p - B1) = 0
        H2 = A2 (r - B3) - A3 (q - B2) = 0.
    This captures ALL fiber points with x != 0 (the pivot is valid on the
    whole stratum), each exactly once.

H1 and H2 share a factor of x (spurious {x=0} line); dividing it out
saturates the system. Fiber points are counted as the C-vector-space
dimension of Q[x,y]/(H1', H2') (Groebner, grevlex): since det JF never
vanishes, F is everywhere a local isomorphism, fibers are reduced, and
the count is exact (cross-checked against the square-free degree of the
x-resultant).

Outputs: [1] the complete fiber over the announced collision target,
exactly; [2] the generic fiber size (degree of F); [3] fiber counts along
a line of targets, locating drops — the non-properness (Jelonek) locus,
necessarily nonempty: a proper Keller map would be a finite covering of
the simply-connected C^3 and hence injective.
"""
import sympy as sp

x, y, z = sp.symbols("x y z")

A1, B1 = (1 + x*y)**3, y**2 * (1 + x*y) * (4 + 3*x*y)
A2, B2 = 3*x*(1 + x*y)**2, y + 3*x*y**2*(4 + 3*x*y)
A3, B3 = -x**3, 2*x - 3*x**2*y

P = A1*z + B1
Q = A2*z + B2
R = A3*z + B3


def xy_system(p, q, r):
    """Saturated bivariate system for the x != 0 stratum."""
    H1 = sp.expand(A1*(r - B3) - A3*(p - B1))
    H2 = sp.expand(A2*(r - B3) - A3*(q - B2))
    while True:
        q1, r1 = sp.div(H1, x, x)
        q2, r2 = sp.div(H2, x, x)
        if r1 == 0 and r2 == 0:
            H1, H2 = sp.expand(q1), sp.expand(q2)
        else:
            break
    return H1, H2


def quotient_dim(polys):
    """dim_C  C[x,y]/(polys)  via grevlex Groebner standard monomials.
    Returns None if the ideal is not zero-dimensional."""
    G = sp.groebner(polys, x, y, order="grevlex")
    if G.exprs == [sp.Integer(1)]:
        return 0
    lms = [sp.Poly(g, x, y).LM(order="grevlex") for g in G.exprs]
    lms = [(int(m.exponents[0]), int(m.exponents[1])) for m in lms]
    bx = min((a for a, b in lms if b == 0), default=None)
    by = min((b for a, b in lms if a == 0), default=None)
    if bx is None or by is None:
        return None
    count = 0
    for i in range(bx):
        for j in range(by):
            if not any(i >= a and j >= b for a, b in lms):
                count += 1
    return count


def fiber_count(p, q, r):
    """Exact number of preimages of (p,q,r)."""
    H1, H2 = xy_system(p, q, r)
    n = quotient_dim([H1, H2])
    base = 1 if r == 0 else 0                 # x = 0 stratum
    return (None if n is None else n + base)


if __name__ == "__main__":
    print("[1] Complete fiber over the announced target (-1/4, 0, 0):")
    H1, H2 = xy_system(sp.Rational(-1, 4), 0, 0)
    sols = sp.solve([H1, H2], [x, y], dict=True)
    pts = [(sp.Integer(0), sp.Integer(0), sp.Rational(-1, 4))]  # x=0 stratum
    for s in sols:
        if s[x] == 0:
            continue
        zval = sp.simplify(((sp.Rational(-1, 4) * 0 + 0 - B3) / A3
                            ).subs({x: s[x], y: s[y]}))  # r=0 here
        pts.append((s[x], s[y], zval))
    for pt in pts:
        img = tuple(sp.simplify(c.subs({x: pt[0], y: pt[1], z: pt[2]}))
                    for c in (P, Q, R))
        print(f"    {pt}  ->  F = {img}")
        assert img == (sp.Rational(-1, 4), 0, 0)
    print(f"    fiber size (complete, exact): {len(pts)}")
    n = fiber_count(sp.Rational(-1, 4), 0, 0)
    print(f"    cross-check via quotient dimension: {n}")

    print("\n[2] Generic fiber size (degree of F), random rational targets:")
    import random
    random.seed(20260722)
    for _ in range(5):
        t = tuple(sp.Rational(random.randint(-40, 40),
                              random.randint(1, 12)) for _ in range(3))
        print(f"    fiber over {t}: {fiber_count(*t)}")

    print("\n[3] Fiber counts along the target line (p, 1, 1) — where the")
    print("    count drops below the generic degree is the non-properness")
    print("    (Jelonek) locus:")
    for pv in (-4, -2, -1, sp.Rational(-1, 2), 0, sp.Rational(1, 4),
               sp.Rational(1, 2), 1, 2, 4):
        print(f"    p = {sp.nsimplify(pv)}: "
              f"{fiber_count(sp.nsimplify(pv), 1, 1)} preimage(s)")
