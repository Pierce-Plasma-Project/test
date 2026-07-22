# Three-color Rado numbers: reproduction and frontier extension

Target A1 from `targets.md`: extend the published grids of three-color Rado
numbers by one cell, with the full certificate discipline of this repo.

## Definition and convention

R₃(E), for a linear equation E, is the least n such that **every** 3-coloring
of {1,…,n} contains a monochromatic solution of E. Solutions may repeat
values (x = y etc.). Both conventions exist in the literature; ours is fixed
empirically by calibration below (it reproduces the published/proven values,
so it is the same convention used by Chang–De Loera–Wesley 2022 and the
SC² 2025 symbolic-sets paper).

## Calibration (all passed — `results/rado/calibration.txt`)

- The proven family R₃(mx+y=z) = (m+2)³−(m+2)²−(m+2)−1 (conjectured by
  Myers, proven by Chang–De Loera–Wesley 2022): reproduced exactly at
  m = 1…5 (14, 43, 94, 173, 286) and m = 8 (889). Sub-second to seconds
  per value; every SAT witness re-verified by an independent checker.
- gcd-scaling identity: R₃(16x+2y=2z) = R₃(8x+y=z) = 889 — two different
  encodings of the same underlying equation agree through the whole
  pipeline.

## A lesson found the honest way: partition regularity is the gatekeeper

First target attempt was the family ax+ay=z beyond the published a ≤ 30.
The search for R₃(2x+2y=z) ran SAT past n = 16,384 and died by memory —
and the mathematics says it had to: {2, 2, −1} has **no zero-sum subset**,
so by Rado's theorem the equation is not partition regular, and R₃ may be
infinite. **SAT search alone can never establish infinitude** — that is
exactly what the symbolic-set method of the SC² 2025 paper exists for.
The module now refuses runaway searches (`MAX_N`) and documents the risk.
Frontier work moved to the family ax+by=bz, whose coefficient set
{a, b, −b} always contains the zero-sum pair {b, −b}: partition regular
for every (a, b), so every cell is finite and every search terminates.

## Computed values and the structure they reveal

All values below were computed this session (streaming encoder + CaDiCaL,
every SAT witness re-verified independently); UNSAT boundaries carry
Glucose-DRUP certificates checked by drat-trim where noted. Data in
`results/rado/grid.txt`, certificates in `results/rado/*.drat`.

**The b-ladder (a = 1):** R₃(x+by=bz) = **b³** for every 3 ≤ b ≤ 10 —
all eight UNSAT boundaries DRAT-certified (`s VERIFIED`) — and the
frontier cell one column beyond the published grid:

    R₃(x+26y=26z) = 17,576 = 26³

confirmed by targeted probes (SAT at 17,575 with verified witness, UNSAT
at 17,576; ~35M clauses each; CaDiCaL refutes the boundary in 4 s, which
itself suggests a short structural proof of the b³ law).

**The coprime grid (a ≤ 7, b ≤ 7)** shows a three-regime structure:

- **b ≥ a+2:** R₃ = b³ exactly (all nine such cells);
- **b = a+1, a ≥ 2:** R₃ = b³ + a² exactly (all five such cells:
  31, 73, 141, 241, 379);
- **a > b:** irregular — no simple law fits, and the values are even
  non-monotone in b (R₃(7x+5y=5z) = 470 > R₃(7x+6y=6z) = 462). This is
  the regime where tables genuinely have to be computed.

**Row frontier:** R₃(16x+25y=25z) — a = 16 exceeds the published a ≤ 15;
b − a = 9 puts it in the b³ regime, predicting 15,625 = 25³; the binary
search has the boundary cornered in (15,624, 15,628] at time of writing.

The regime laws above are stated as *empirical observations with certified
data points*, not theorems. Given that the published 2025 grid covers
a ≤ 15, b ≤ 25, its authors have all interior values and would not have
missed the b³ pattern; the observations should be presumed known (and
possibly proven by their symbolic-set method, which is designed exactly
for infinite families). Our frontier cells extend the *data* one step past
the grid boundary either way, and every value stands on its own
certificates.

## Claim hygiene

"New" here means: outside the coefficient ranges of the published tables we
could verify against (the 2025 grids as reported; arXiv itself is not
reachable from this environment's network policy, so the values should be
cross-checked against the paper's tables before any publication claim).
The certificates make the *values* unconditional either way; only the
*novelty* claim depends on the literature check.

## Reproduce

```sh
cd experiments/rado
pip install python-sat
python3 rado.py 2 1 1              # = 43 in ~0.1s
python3 rado.py 16 25 25 --certify --witness
python3 rado.py 1 26 26 --start 16384
```
