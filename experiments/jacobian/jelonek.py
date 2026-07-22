#!/usr/bin/env python3
"""Exact computation of the non-properness (Jelonek) locus of the
Alpoge/Fable 5 Keller map F.

Method: for symbolic target (p,q,r), the x != 0 fiber x-coordinates are
roots of W(x) = Res_y(H1, H2) (after stripping the generic power of x).
Fiber points escape when a root of W leaves to x = infinity (leading
coefficient of W vanishes) or migrates to x = 0, where z = (r-B3)/(-x^3)
blows up (trailing coefficient vanishes). The Jelonek set is contained in
the union of these two coefficient hypersurfaces; probe points from
anatomy.py discriminate which components are genuine.
"""
import sympy as sp

x, y = sp.symbols("x y")
p, q, r = sp.symbols("p q r")

A1, B1 = (1 + x*y)**3, y**2 * (1 + x*y) * (4 + 3*x*y)
A2, B2 = 3*x*(1 + x*y)**2, y + 3*x*y**2*(4 + 3*x*y)
A3, B3 = -x**3, 2*x - 3*x**2*y

H1 = sp.expand(A1*(r - B3) - A3*(p - B1))
H2 = sp.expand(A2*(r - B3) - A3*(q - B2))
print(f"deg_y H1 = {sp.degree(H1, y)}, deg_y H2 = {sp.degree(H2, y)}")

W = sp.expand(sp.resultant(H1, H2, y))
Wp = sp.Poly(W, x)
coeffs = Wp.all_coeffs()[::-1]                    # ascending in x
# strip x^k where the coefficient is *identically* zero
k = 0
while sp.expand(coeffs[k]) == 0:
    k += 1
stripped = coeffs[k:]
print(f"W = Res_y(H1,H2): degree {Wp.degree()} in x, "
      f"x^{k} factors out identically")

lead = sp.factor(sp.expand(stripped[-1]))
trail = sp.factor(sp.expand(stripped[0]))
print(f"\nleading x-coefficient (escape to x=inf):\n  {lead}")
print(f"\ntrailing x-coefficient (escape via x->0, z->inf):\n  {trail}")

probes_drop = [(0, 1, 1), (0, 0, 1), (0, 0, 0)]        # count dropped to 1
probes_full = [(0, 2, 5), (0, -3, 2), (0, 1, 0),        # count stayed 3
               (sp.Rational(1, 100), 1, 1), (1, 1, 1)]
print("\nprobe consistency (drop points must lie on a candidate component,")
print("full-count points must avoid the genuine ones):")
for tgt in probes_drop + probes_full:
    sub = {p: tgt[0], q: tgt[1], r: tgt[2]}
    print(f"  {tgt}: lead = {sp.expand(lead.subs(sub))}, "
          f"trail = {sp.expand(trail.subs(sub))}")
