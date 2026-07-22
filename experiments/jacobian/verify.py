#!/usr/bin/env python3
"""Independent exact verification of the Alpoge / Claude Fable 5
counterexample to the Jacobian conjecture (announced July 20, 2026).

Claimed: F = (P, Q, R) : C^3 -> C^3 with
    P = (1+xy)^3 z + y^2 (1+xy)(4+3xy)
    Q = y + 3x (1+xy)^2 z + 3x y^2 (4+3xy)
    R = 2x - 3x^2 y - x^3 z
has det JF identically -2 (a nonzero constant, so F is a Keller map), yet
maps the three distinct points
    (0, 0, -1/4), (1, -3/2, 13/2), (-1, 3/2, 13/2)
to the same output (-1/4, 0, 0) — hence F is not injective, contradicting
the Jacobian conjecture for n = 3 (and, adjoining identity coordinates,
for every n >= 3).

Everything below is exact rational/symbolic arithmetic (sympy). Exit code 0
iff every claim checks.
"""
import sympy as sp

x, y, z = sp.symbols("x y z")

P = (1 + x*y)**3 * z + y**2 * (1 + x*y) * (4 + 3*x*y)
Q = y + 3*x * (1 + x*y)**2 * z + 3*x * y**2 * (4 + 3*x*y)
R = 2*x - 3*x**2*y - x**3*z

F = sp.Matrix([P, Q, R])
J = F.jacobian([x, y, z])

det = sp.expand(J.det())
print(f"det JF = {det}")
assert det == -2, "FAILED: Jacobian determinant is not identically -2"

pts = [(sp.Integer(0), sp.Integer(0), sp.Rational(-1, 4)),
       (sp.Integer(1), sp.Rational(-3, 2), sp.Rational(13, 2)),
       (sp.Integer(-1), sp.Rational(3, 2), sp.Rational(13, 2))]
target = (sp.Rational(-1, 4), sp.Integer(0), sp.Integer(0))

assert len(set(pts)) == 3, "FAILED: points are not distinct"
for p in pts:
    img = tuple(sp.simplify(c.subs({x: p[0], y: p[1], z: p[2]})) for c in (P, Q, R))
    print(f"F{p} = {img}")
    assert img == target, f"FAILED: F{p} != {target}"

print()
print("VERIFIED: det JF == -2 identically (Keller map), and three distinct")
print("points share one image. F is a non-injective Keller map: the")
print("Jacobian conjecture is FALSE for n = 3 (hence all n >= 3).")
