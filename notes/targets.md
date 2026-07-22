# Vetted targets for the certificate-search pipeline

Status verified against the live literature, July 2026. Selection criteria,
in order: (1) the answer is a finite object a machine can verify
unconditionally; (2) a single machine — or one machine plus patience — still
moves the frontier, i.e. the problem is not yet a CPU-century wall;
(3) there is an active venue tracking and crediting exactly this kind of
contribution; (4) our existing skeleton (encoder → solver → DRAT/enumeration
certificate) extends to it without a new science.

**Context worth stating.** The "models are making amazing math progress"
trend is real and now precisely documented — and it lives exactly in this
tier, not at the millennium tier. Tao's public tracker records AI
contributions moving on the order of a hundred Erdős problems to solved
status since October 2025, with the first autonomous nontrivial AI solution
of an Erdős problem in January 2026 (Problem #728, informal argument by a
frontier LLM, formalized in Lean by Harmonic's Aristotle) and, notably, an
AI disproof of Erdős's planar unit-distance conjecture. The tracker's own
breakdown confirms the pattern this repo is built around: full AI solutions
concentrate on combinatorial problems with finite verification; deep
structural theory remains closed. Aim where the trend actually is.

---

## Tier A — aim now (single machine, days-to-weeks per result)

### A1. Three-color Rado numbers (top recommendation)
Rado numbers R₃(E) — the smallest N such that every 3-coloring of {1..N}
contains a monochromatic solution of a given linear equation E — are an
active SAT frontier: the 2022 Chang–De Loera–Wesley line and the 2025
symbolic-sets work (Bright et al., SC² 2025) computed grids of previously
unknown exact values, the largest single instance taking ~59 hours on
ordinary hardware (293k variables, 256M clauses). There is an effectively
infinite supply of unknown values *adjacent to the published tables*, the
encoding is a direct generalization of our Schur/Ramsey encoder, and
upper-bound proofs are UNSAT results that carry DRAT certificates — our
exact pipeline. **First shot:** reproduce a published value end-to-end with
a verified certificate, then extend one family (e.g. R₃(ax+by=bz) grids)
one step beyond the published range. Each new value is a small, real,
publishable fact.

### A2. Mixed van der Waerden numbers w(2; 3, t)
Exact values stop at w(2;3,19) = 349 (Ahmed–Kullmann–Snevily), with
w(2;3,20) ≥ 389 conjectured sharp, and lower bounds only for t up to 39.
Two contribution modes: (i) improved lower bounds via good-partition search
(pure witness-finding — cheap, certificate = the partition itself);
(ii) the crown: close w(2;3,20) = 389 by UNSAT at n = 389 — a
cube-and-conquer campaign (this problem family is literally where
cube-and-conquer was invented). Mode (i) is a weekend; mode (ii) is a
long-running background project with a named payoff.

### A3. Erdős catalogue, finite-core items
The documented lane of the AI-in-math trend. Filter erdosproblems.com for
open items whose content is a finite search or an explicit construction
(the tracker's full-solution list — #38, #90, #125, #205, #457, #694, #960,
#987, #990, #1014, #1091, #1141, #1196, #1202, #1217… — shows the shape
that falls). Contribution modes, in ascending ambition: computational
verification extending known ranges; counterexample search; a construction
plus machine check; an informal proof handed to Lean formalization. Every
contribution type is explicitly tracked and credited on the wiki.

## Tier B — aim with more compute or a new trick (months)

### B1. Size-optimal sorting network, n = 13
Optimal comparator counts are known only through small n; for 13 channels
the gap is 45 vs 46. This is a named, decades-old open value (Knuth-tier
provenance), SAT-shaped, with published methodology (Codish et al.) that
stalls exactly at n = 13. A contribution here is a symmetry-breaking /
prefix-pruning insight, not raw compute — the same lesson our K₁₈
experiment measured (unbounded → 0.17 s from two WLOG clauses).

### B2. Smaller 5-chromatic unit-distance graphs
Record: 509 vertices (Parts, from the de Grey → Heule 553 line; SAT
trimming with clausal-proof optimization is the established method). The
plane record is polished, but the variant space is live — de Grey published
a 61-vertex triangle-free 3D construction in 2026, and sphere-radius and
odd-distance variants have open records. Our pipeline needs one new layer:
a geometric graph generator feeding the SAT trimmer.

### B3. Small matrix-multiplication schemes via flip graphs
The hottest of the accessible frontiers — symmetric flip graphs improved
5×5×5 and 6×6×6 in 2025, the meta flip graph (June 2026) improved ranks for
207 formats. Single-workstation runs have repeatedly set records here. The
catch: it is now crowded, and records move monthly — expect competition.
Certificates are trivial (a scheme verifies by multiplying it out).

### B4. Schur / weak-Schur templates
S(6) exact is hopeless (S(5) = 160 took 2 PB of proof), but *template*
constructions driving lower-bound recurrences are moving right now (July
2026: shifted S-templates improved S(k+2) ≥ 10S(k)+2 over Abbott–Hanson).
Template search is witness-search — our cheap direction.

## Tier C — walls (build infrastructure, do not promise results)

- **R(5,5) ∈ [43, 46]** (Angeltveit–McKay ≤ 46, reconfirmed 2025): each
  further vertex is CPU-decades plus gluing theory. Our encoder generates
  the instance; nobody's solver finishes it.
- **S(6), g(7) (Erdős–Szekeres), projective plane of order 12**: same
  shape, bigger walls.
- The honest role for these: test articles for encoding and decomposition
  ideas, benchmarked against the sym-broken-vs-raw ratio we measured.

## Recommended sequence

1. **A1 now**: Rado-number module on the existing skeleton; reproduce one
   published value with a drat-trim-verified certificate; extend one grid
   cell. Deliverable: new exact values + certificates.
2. **A2 mode (i)** in parallel as background compute: good-partition search
   for w(2;3,t), t = 20..39, targeting lower-bound improvements.
3. Pick one **A3** Erdős item with a finite core as the "named problem"
   flag on the mast.
4. Revisit B1 only with a concrete symmetry-breaking idea in hand.

## Sources

- Tao's tracker: github.com/teorth/erdosproblems/wiki (AI contributions),
  erdosproblems.com
- Erdős #728 resolution writeup: arXiv:2601.07421
- Rado/SAT: arXiv:2210.03262; symbolic sets, SC² 2025: arXiv:2505.12085
- vdW w(2;3,t): arXiv:1102.5433 (values/conjectures); lower bounds
  arXiv:2102.01543, arXiv:2111.01099
- R(3,8)/R(3,9) verified certificates: arXiv:2502.06055
- R(5,5) ≤ 46: arXiv:2409.15709
- Sorting networks: arXiv:1405.5754, arXiv:1501.06946, arXiv:1507.01428
- 5-chromatic UD graphs: arXiv:1805.12181, arXiv:1907.00929,
  arXiv:2010.12665
- Flip graphs: arXiv:2212.01175; meta flip graph arXiv:2510.19787,
  arXiv:2606.02480
- Schur templates: arXiv:2607.15034; S(7) templates arXiv:2107.03560
