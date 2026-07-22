# Anatomy of the first counterexample to the Jacobian conjecture

On July 20, 2026, Levent Alpöge announced a counterexample to the Jacobian
conjecture, credited to Claude Fable 5: the Keller map F = (P,Q,R) on ℂ³,

    P = (1+xy)³z + y²(1+xy)(4+3xy)
    Q = y + 3x(1+xy)²z + 3xy²(4+3xy)
    R = 2x − 3x²y − x³z

with det JF ≡ −2, mapping (0,0,−1/4), (1,−3/2,13/2), (−1,3/2,13/2) to the
common image (−1/4,0,0). This kills the conjecture for all n ≥ 3; n = 2
remains open. `experiments/jacobian/verify.py` re-verifies both claims in
exact arithmetic (`results/jacobian_verification.txt`).

This note records a computational dissection of the specimen
(`anatomy.py`, `jelonek.py`), done in exact arithmetic two days after the
announcement. Everything below is a computed fact unless marked as
interpretation.

## Structure: F is affine in z

F = A(x,y)·z + B(x,y) with A = ((1+xy)³, 3x(1+xy)², −x³). Consequences:

- **The plane {x=0} maps bijectively onto the plane {r=0}**:
  F(0,y,z) = (z+4y², y, 0). This single stratum explains the first
  collision point.
- For x ≠ 0, z = (r−B₃)/(−x³) is determined, and fibers reduce to a
  bivariate system (H₁, H₂) — sharing an identical factor x that
  saturation removes. All fiber counts below are exact (Gröbner quotient
  dimension; fibers are reduced since det JF never vanishes).

## Computed facts

1. **The announced collision is the entire fiber.** The preimage of
   (−1/4, 0, 0) is exactly the three announced points — no others.

2. **F has degree 3.** Every random target probed has exactly 3
   preimages: F is 3-to-1 almost everywhere, not just at the exhibited
   point. Equivalently [ℂ(x,y,z) : ℂ(P,Q,R)] = 3. Cross-check: the
   eliminant Res_y(H₁,H₂) is x¹²·(cubic in x) identically — the fiber
   cubic.

3. **The non-properness (Jelonek) locus is the quartic surface**

       S : 27p²r² − 18pqr + 16p + q³r − q² = 0.

   Derivation: fiber points can only escape to x = ∞ (leading coefficient
   of the fiber cubic vanishes) or to x → 0 with z → ∞ (trailing
   coefficient vanishes). The leading coefficient factors as
   r·(27p²r² − 18pqr + 16p + q³r − q²); the trailing one is −2r². The
   r-factors are pivot artifacts — over {r=0} the migrating root is the
   genuine x=0-stratum fiber point, and counts stay full — leaving S as
   the escape locus.
   **Prediction test (pre-registered):** S contains the rational family
   (0, q, 1/q); all four probed members drop to 1 preimage, while all
   control targets off S have exactly 3. Fibers over S drop by two — the
   escapes occur in pairs.

4. **Global picture** (computed facts + standard theory): F is an
   everywhere-unramified, non-proper, degree-3 polynomial self-map of ℂ³:
   a 3-sheeted covering space of ℂ³ ∖ S. Non-properness is forced: a
   proper Keller map would be a finite covering of simply-connected ℂ³,
   hence injective. The specimen realizes the escape mechanism with the
   minimal possible structural budget — one spare dimension used affinely,
   escapes running x → 0, y ~ −1/x, z ~ −r/x³ along the asymptotic curve
   xy → −1.

## Interpretation (clearly labeled)

- The z-affine trick needs n ≥ 3: the map is a z-parametrized family of
  plane maps. Nothing of this shape exists in the plane, which is
  consistent with n = 2 surviving.
- The fiber count over S drops 3 → 1, never 3 → 2, over every probed
  point of S — the two non-stratum sheets are exchanged/escape together.
  (Fact for probed points; the "never" is interpretation.)
- Status of these observations: computed independently here from the
  specimen; the announcement is 48 hours old and the consequences
  literature (e.g., Zhang's preprint) is moving fast, so overlap is
  likely and priority is neither claimed nor important. The value is the
  verified, reproducible record: every claim above regenerates from
  `verify.py`, `anatomy.py`, `jelonek.py` in minutes.

## Reproduce

```sh
cd experiments/jacobian
pip install sympy
python3 verify.py     # det JF = -2, the 3-point collision (exact)
python3 anatomy.py    # complete fibers, degree 3, drop probes (exact)
python3 jelonek.py    # the quartic non-properness locus (exact)
```
