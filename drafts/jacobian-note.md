# The geometry of the Alpöge–Fable Keller map: degree three and an explicit non-properness quartic

*Draft research note — computations by Claude (Anthropic), July 22–23,
2026, from the counterexample announced July 20. All computations are
exact (sympy, rational/symbolic arithmetic) and regenerate in minutes
from three short scripts. The object is days old and under intense
parallel study; no priority is claimed. Facts and interpretation are
separated throughout.*

## 0. The specimen

F = (P, Q, R) : ℂ³ → ℂ³ (Alpöge, July 20, 2026; found with Claude
Fable 5):

    P = (1+xy)³z + y²(1+xy)(4+3xy)
    Q = y + 3x(1+xy)²z + 3xy²(4+3xy)
    R = 2x − 3x²y − x³z

Verified exactly (`verify.py`): det JF ≡ −2, and the three distinct
points (0,0,−1/4), (1,−3/2,13/2), (−1,3/2,13/2) share the image
(−1/4,0,0). Hence F is a non-injective Keller map: the Jacobian
conjecture fails for all n ≥ 3.

## 1. Structural observation: F is affine in z

F = A(x,y)·z + B(x,y), A = ((1+xy)³, 3x(1+xy)², −x³). Two consequences,
both exact:

- **(P1)** F restricts to a bijection {x=0} → {r=0}: F(0,y,z) =
  (z+4y², y, 0), inverse (p,q,0) ↦ (0, q, p−4q²).
- For x ≠ 0, z = (r−B₃)/(−x³), and fibers reduce to a bivariate system;
  after saturating an identical factor x, fiber points are counted by
  Gröbner quotient dimension. Counts are exact: det JF never vanishes,
  so F is everywhere a local isomorphism and all fibers are reduced.

## 2. Computed facts

- **(P2) The announced collision is the complete fiber.**
  F⁻¹(−1/4, 0, 0) is exactly the three announced points.
- **(P3) F has degree 3.** Every sampled target (random rational, and
  all probes below off the surface S) has exactly 3 preimages;
  structurally, Res_y of the saturated system equals x¹²·(cubic in x)
  identically in the target, so the x ≠ 0 fiber is governed by a cubic:
  [ℂ(x,y,z) : ℂ(P,Q,R)] = 3.
- **(P4) Non-properness locus.** Fiber points can leave only via
  x → ∞ (leading coefficient of the fiber cubic vanishes) or via x → 0,
  z → ∞ (trailing coefficient vanishes). Computed exactly:

      leading coefficient  = r · (27p²r² − 18pqr + 16p + q³r − q²)
      trailing coefficient = −2r²

  so the non-properness set S_F satisfies
  **S_F ⊆ S ∪ {r=0}**, with S : 27p²r² − 18pqr + 16p + q³r − q² = 0.
  The {r=0} component is a pivot artifact: by (P1) the migrating root at
  x=0 is a genuine fiber point on the x=0 stratum, and all probed
  targets in {r=0}∖S have full count 3. **Pre-registered validation of
  S:** the rational family (0, q, 1/q) ⊂ S was predicted to drop and
  does (fiber count 1 at all four probed members: q = 2, 3, 1/2, −5);
  control targets off S all have exactly 3. Conclusion (computed
  containment + sampled equality): **S_F = S, an explicit quartic
  hypersurface.**
- **(P5, observation)** At every probed point of S the count drops
  3 → 1, never 3 → 2: the two non-stratum sheets escape together, along
  x → 0, y ~ −1/x, z ~ −r/x³ (asymptote xy → −1).

## 3. Global picture

F is an everywhere-unramified, non-proper, degree-3 polynomial self-map
of ℂ³ — a 3-sheeted covering space of ℂ³ ∖ S. Non-properness is forced
for any non-injective Keller map (a proper one would finitely cover
simply-connected ℂ³, hence be injective); the specimen realizes it with
minimal structural budget: one spare dimension, used affinely.
Interpretation: the z-affine mechanism — a z-parametrized family of
plane maps — has no analogue in ℂ², consistent with the survival of the
two-dimensional case.

## 4. What to cross-check before treating anything here as new

The counterexample thread (Secret Blogging Seminar, July 20+) and the
consequences preprints (e.g., Zhang, "Direct consequences...") may
already contain the degree computation and/or the quartic S. If S
appears nowhere: an explicit equation for the non-properness locus of
the first Jacobian counterexample is a small new fact about an important
object. Either way, the computations are independent and reproducible:

```sh
cd experiments/jacobian && pip install sympy
python3 verify.py     # det JF = -2; the 3-point collision (exact)
python3 anatomy.py    # complete fibers, degree 3, drop probes (exact)
python3 jelonek.py    # leading/trailing coefficients, the quartic S (exact)
```

Transcripts: `results/jacobian_verification.txt`,
`results/jacobian_anatomy.txt`, `results/jacobian_jelonek.txt`.
