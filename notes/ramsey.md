# R(4,4) = 18, reproduced end-to-end with machine-verified leaves

This is the working skeleton of the *certificate-search* genre discussed in
`frontier.md` §8 and in the famous-problems survey: the lane where AI/search
systems have genuinely "picked away" at famous mathematics (Boolean
Pythagorean triples 2016, Schur number 5 2017, Keller dim 7 2020, empty
hexagon 2024, chromatic number of the plane ≥ 5 2018). The pattern is always:
**encoder → search → independently checkable certificate**. Here that pattern
is built and exercised on a classical target whose answer is known — which is
exactly what you want from a skeleton: every step is checkable against ground
truth before the same tooling is pointed at anything open.

The claim being reproduced: **R(4,4) = 18** (Greenwood–Gleason 1955) — every
2-coloring of the edges of K₁₈ contains a monochromatic K₄, and there is a
2-coloring of K₁₇ with none.

## The chain, and what is machine-verified where

Notation: Ramsey(s,t;n) is the CNF over the C(n,2) edge variables of Kₙ
(true = red) forbidding every red Kₛ and every blue Kₜ. Satisfiable iff
R(s,t) > n. All artifacts regenerate from source; outputs in
`results/ramsey/`.

### R(3,3) = 6  — the base leaf
- **Lower (R(3,3) > 5):** pentagon coloring of K₅ verified to have no
  monochromatic triangle (all 10 triangles checked). `enumerate.c [A]`
- **Upper (R(3,3) ≤ 6):** *all* 2¹⁵ = 32,768 colorings of K₆ enumerated;
  every one contains a monochromatic triangle. `enumerate.c [B]`
  Independently: Ramsey(3,3;6) proved UNSAT by the homemade DPLL
  (19 decisions) and by CaDiCaL.

### R(3,4) = 9  — the load-bearing leaf
- **Lower (R(3,4) > 8):** *all* 2²⁸ = 268,435,456 colorings of K₈
  enumerated; exactly **17,640** avoid both a red K₃ and a blue K₄
  (witnesses exist; one is printed, and the classical cyclic witness
  C₈(1,4) is verified separately). `enumerate.c [C]`
- **Upper (R(3,4) ≤ 9):** Ramsey(3,4;9) — 36 variables, 210 clauses —
  proved UNSAT by the homemade DPLL in 19,563 decisions and by CaDiCaL in
  ~0.1 s. Two solvers, written independently, one of them 60 lines of
  from-scratch C.
- **Human-checkable backup** (so the SAT leg is not a trust bottleneck),
  the classical parity argument: in a mono-K₃/K₄-free coloring of K₉, no
  vertex has red-degree ≥ 4 (else among those 4 red neighbors, a red edge
  makes a red K₃ with the center, and no red edge makes a blue K₄), and no
  vertex has blue-degree ≥ 6 (else R(3,3) = 6 applied to those 6 neighbors
  yields a red K₃ outright or a blue K₃ completing a blue K₄). So all nine
  vertices have red-degree exactly 3 — but a 3-regular graph on 9 vertices
  is impossible, since degree sums are even. ∎

### R(4,4) = 18  — the target
- **Lower (R(4,4) > 17):** the Paley graph on ℤ₁₇ (edge red iff the
  difference is a nonzero quadratic residue mod 17; well-defined since
  −1 ≡ 16 is a QR) has **no monochromatic K₄**: all C(17,4) = 2,380
  four-subsets checked, 0 red, 0 blue. `enumerate.c [D]`
  Independently: CaDiCaL finds a witness for Ramsey(4,4;17) by search, and
  the model is re-verified by a separate checker before being reported.
- **Upper (R(4,4) ≤ 18), route 1 — classical reduction:** in any 2-colored
  K₁₈ take a vertex v; of its 17 edges, some color — WLOG red — appears ≥ 9
  times. Among those 9 red neighbors, R(3,4) ≤ 9 (machine-verified leaf
  above) forces a red K₃ (completing a red K₄ with v) or a blue K₄. ∎
  Every leaf of this argument is machine-verified; the glue is the two
  sentences you just read.
- **Upper, route 2 — direct SAT:** Ramsey(4,4;18) — 153 variables, 6,120
  clique clauses — attacked directly with CaDiCaL, in two forms:
  raw, and with the two-step symmetry breaking described below.
  Results in `results/ramsey/sat_check.txt`. The naive homemade DPLL does
  **not** finish this instance in 120 s — an honest demonstration of why
  clause learning matters; see "the gap between naive and modern" below.

## Soundness of the symmetry breaking (why "WLOG" is valid in CNF)

For s = t the constraint set is invariant under (a) swapping the two colors
and (b) permuting vertices. Given any solution of the raw formula:
by (a) we may assume vertex 0 has red-degree ≥ ⌈17/2⌉ = 9; by (b) — a
permutation of vertices 1..17 only — we may sort row 0 so its red edges form
a prefix. Both maps send solutions to solutions, so adding the clauses
"x₀,ⱼ₊₁ → x₀,ⱼ" plus the unit "x₀,₉" preserves satisfiability, and
unsatisfiability of the constrained formula still proves R(4,4) ≤ 18.
(With the prefix ordering, "red-degree ≥ 9" is exactly the single literal
x₀,₉ — no cardinality encoding needed.) Note the constrained instance embeds
the R(3,4;9) subproblem on vertices 1..9, which is why it collapses quickly.

## Independence of the verification paths

Three legs that share no code: (1) exhaustive enumeration in C over raw
bitmasks; (2) a from-scratch DPLL reading DIMACS; (3) industrial CDCL
solvers — with every SAT model re-checked by a fourth, separate model
verifier before being believed. A bug would have to be replicated across
independent implementations to survive.

For UNSAT results the trust gap — "the solver says so" — is closed with
proof certificates: DRUP proofs for Ramsey(3,4;9) and the sym-broken
Ramsey(4,4;18) were emitted (Glucose 3 via python-sat), and both were
**verified by `drat-trim`** — the official checker behind the 200 TB
Pythagorean-triples certificate — compiled from source in-session
(`results/ramsey/*.drat`, both `s VERIFIED`). An instructive wrinkle: the
first certificates, exported through CaDiCaL's pysat proof tracer, ended in
a claimed conflict that drat-trim could **not** reproduce (`s NOT
VERIFIED`) — an incomplete proof export, caught exactly the way this
methodology is designed to catch things. The proofs were re-emitted through
Glucose's DRUP path and passed. Moral: in this genre, unchecked output —
whether from a language model or an industrial SAT solver — is not a
result. The checker is the result.

## The gap between naive and modern (measured here)

| instance | homemade DPLL | CaDiCaL (CDCL) |
|---|---|---|
| Ramsey(3,3;6) | UNSAT, 19 decisions | UNSAT, <0.01 s |
| Ramsey(3,4;9) | UNSAT, 19,563 decisions | UNSAT, 0.10 s |
| Ramsey(4,4;17) | (not attempted) | SAT witness, 0.04 s |
| Ramsey(4,4;18) sym-broken | timeout @ 120 s | **UNSAT, 0.17 s** |
| Ramsey(4,4;18) raw | (hopeless) | undecided after 2×10⁷ conflicts, 1304 s |

The last two rows are the sharpest lesson in the table: the *same* solver,
on the *same* theorem, goes from >21 minutes and still undecided to 0.17
seconds when two WLOG constraints (17 sorting clauses + 1 unit) are added.
Symmetry is the wall, and encoding insight — not raw compute — is what
moves it. Scaled up, that ratio is why R(5,5) is attacked with full
lex-leader breaking and cube-and-conquer rather than bigger machines alone.

Same formulas, same machine. Clause learning, watched literals, restarts,
and activity heuristics are the difference — this measured gap, scaled up,
is the entire modern certificate-search genre.

## Pointing the same skeleton at open problems

The encoder generalizes with no new code (`encode.py s t n`):

- **R(5,5)**, open, known to lie in **[43, 46]**: `encode.py 5 5 43
  --stats-only` → 903 variables, **1,925,196 clauses**. The formula exists;
  the wall is search: 2⁹⁰³ raw space, and the state of the art
  (McKay–Angeltveit-style gluing + cube-and-conquer SAT with full
  lex-leader symmetry breaking) spends CPU-*centuries* per shaved vertex.
  Any real contribution here is a better encoding, a stronger symmetry
  break, or a new gluing decomposition — not a bigger machine alone.
- Nearer, genuinely pickable targets in the same genre: smaller unit-distance
  graphs for the chromatic number of the plane (5 ≤ χ ≤ 7), order-type
  searches à la the empty-hexagon result, Schur/Rado-style colorings, and
  the concrete combinatorial items in the Erdős catalogue.

## Reproduce

```sh
cd experiments/ramsey
gcc -O2 -o enumerate enumerate.c && ./enumerate          # ~2.5 s, all leaves
gcc -O2 -o solver solver.c
python3 encode.py 3 4 9 --out r34_9.cnf && ./solver r34_9.cnf
pip install python-sat && python3 sat_check.py           # CDCL leg
# certificate check: build drat-trim (single C file, github.com/marijnheule)
# then: drat-trim r34_9.cnf ../../results/ramsey/r34_9.drat  -> "s VERIFIED"
```
