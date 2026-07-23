# Three-color Rado numbers of ax+by=bz beyond the published grids: certified values and empirical regime laws

*Draft research note — computations by Claude (Anthropic), July 22, 2026.
All values carry machine-checked certificates; novelty claims are
conditional on the literature check in §6, which requires access to
arXiv:2210.03262 (Chang–De Loera–Wesley) and arXiv:2505.12085
(symbolic sets, SC² 2025) — not reachable from the computing
environment.*

## 1. Setup

For a linear equation E, the three-color Rado number R₃(E) is the least n
such that every 3-coloring of {1,…,n} contains a monochromatic solution
of E; solutions may repeat values. The family here is
E(a,b): ax + by = bz, which is partition regular for every a, b (the
coefficient set {a, b, −b} contains the zero-sum pair), so every R₃ is
finite. Cells with gcd(a,b) = g reduce to (a/g, b/g); only coprime cells
are primitive.

Calibration of the pipeline against proven values (Myers' conjecture,
proven by Chang–De Loera–Wesley 2022): R₃(mx+y=z) = (m+2)³−(m+2)²−(m+2)−1
reproduced exactly at m = 1,2,3,4,5,8 (14, 43, 94, 173, 286, 889), and
the gcd-scaling identity R₃(16x+2y=2z) = R₃(8x+y=z) = 889 confirmed.

## 2. Method

Direct 3n-variable CNF encoding (one-hot colors; one clause per solution
triple per color), streamed into the solver. Boundary located by
exponential + binary search with CaDiCaL 1.9.5; every SAT witness
re-verified by an independent checker sharing no code with the encoder;
every UNSAT boundary re-proved by Glucose 3 with DRUP proof logging and
the proof verified by drat-trim (`s VERIFIED` in all cases below).
Certificates and extremal colorings: `results/rado/` in the repository;
everything regenerates via `experiments/rado/rado.py`.

## 3. Computed values

**Reproductions / interior cells** (within published ranges; for
cross-checking): the coprime grid a ≤ 7, b ≤ 7 and the ladder
R₃(x+by=bz) = b³ for 3 ≤ b ≤ 10 (each of these eight UNSAT boundaries
DRAT-certified). Full table in `results/rado/grid.txt`.

**Beyond-grid cells** (candidate-new; published grids reported as
a ≤ 15, 1 ≤ b ≤ 25 for this family):

| cell | value | structure |
|---|---|---|
| R₃(16x+3y=3z) | 4465 | = a³+a²+a+1+2ab |
| R₃(16x+5y=5z) | 4529 | = a³+a²+a+1+2ab |
| R₃(16x+7y=7z) | 4593 | = a³+a²+a+1+2ab |
| R₃(16x+9y=9z) | 4657 | = a³+a²+a+1+2ab |
| R₃(16x+11y=11z) | 4576 | transition zone (no law) |
| R₃(16x+13y=13z) | 4602 | transition zone |
| R₃(16x+15y=15z) | 4620 | transition zone |
| R₃(16x+17y=17z) | 5169 | = 17³+16², predicted before computing |
| R₃(16x+19y=19z) | 6859 | = 19³, predicted before computing |
| R₃(16x+25y=25z) | 15625 | = 25³ |
| R₃(x+26y=26z) | 17576 | = 26³ |

## 4. Empirical regime laws (coprime a, b; b ≥ 2)

- **b ≥ a+2:** R₃ = b³ (eleven cells, no exceptions).
- **b = a+1, a ≥ 2:** R₃ = b³ + a² (six cells: 31, 73, 141, 241, 379, 5169).
- **2 ≤ b ≲ 0.6a:** R₃ = a³+a²+a+1+2ab. Fitted on the a=16 row
  (b = 3,5,7,9), retrodicts the a=7 row exactly (400+14b at b = 2..5).
- **0.6a ≲ b ≤ a−1:** irregular and non-monotone
  (R₃(7x+5y=5z) = 470 > R₃(7x+6y=6z) = 462; the a=16 row dips at b=11).

The b=17 and b=19 rows of the table were predictions from these laws made
before computation; both hit exactly. Stated as empirical observations
with certified data points, not theorems. The b³ lower bound has a
natural witness shape (one color = non-multiples of b, forced safe since
ax ≡ 0 mod b in any solution; multiples of b recursively 2-colored),
suggesting the laws are provable by the symbolic-set method.

## 5. A caution for SAT search on this family's cousins

The family ax+ay=z (a ≥ 2) has coefficient set {a, a, −1} with no
zero-sum subset: not partition regular, so R₃ may be infinite, and SAT
search alone cannot establish that (R₃(2x+2y=z) verified SAT past
n = 16,384 with no boundary in sight). Infinite-family methods
(symbolic sets) are the right tool there.

## 6. Novelty checklist (5 minutes with the two papers)

1. In arXiv:2505.12085, confirm the computed grid for R₃(ax+by=bz) is
   exactly 1 ≤ a ≤ 15, 1 ≤ b ≤ 25, and whether any table or theorem
   states the b³, b³+a², or a³+a²+a+1+2ab laws.
2. In arXiv:2210.03262, check the tables for any (a,b) cells of this
   family with a ≥ 16 or b = 26.
3. If neither contains them: the eleven beyond-grid values in §3 are new
   computed Rado numbers, and the §4 laws are (at minimum) new as
   explicit statements. If either does: the values become independent
   certified confirmations; the certificates retain their value either way.

## 7. Data availability

Branch `claude/p-np-problem-testing-ulks43` of the repository:
`experiments/rado/rado.py` (pipeline), `results/rado/*.drat` (DRUP
certificates, all drat-trim VERIFIED), `results/rado/grid.txt` (values),
`results/rado/calibration.txt` (calibration transcript).
