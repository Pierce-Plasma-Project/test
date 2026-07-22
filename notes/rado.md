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

## Frontier cells (beyond the published grid a ≤ 15, 1 ≤ b ≤ 25)

Primitive cells require gcd(a, b) = 1 (otherwise the equation reduces by
gcd to an interior cell — that identity is itself validated above).

Cells being computed, streaming encoder + CaDiCaL, witness-checked at every
SAT step, Glucose-DRUP + drat-trim certification at the UNSAT boundary:

- **R₃(16x+25y=25z)** — row frontier (a = 16 exceeds the published a ≤ 15).
- **R₃(x+26y=26z)** — column frontier (b = 26 exceeds the published
  b ≤ 25). Known SAT at n = 16,384; boundary in the ≥ 17,000 range with
  ~3–6×10⁷ clauses per instance at the top of the search.

Results land in `results/rado/` as they complete: value, extremal
coloring, CNF, DRUP proof, checker verdict.

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
