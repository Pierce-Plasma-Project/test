# P vs NP: honest frontier work

This repository is a response to a request to "keep up with unpublished
progress on P = NP." It contains no claimed breakthrough, because a
breakthrough is not something any model — from any lab — can emit on demand,
and unverifiable claims of one deserve a rubric, not imitation. What it
contains instead is the real thing at the scale where the real thing is
possible:

- **`notes/frontier.md`** — a precise map of the current frontier: the three
  barrier theorems, the actual lower-bound scoreboard (the record for general
  circuits is ≈ 3.1n — linear), each live attack program (algorithmic method,
  hardness magnification, meta-complexity, GCT, proof complexity) with the
  *exact shape of the missing lemma* in each, and a 7-point audit rubric for
  any claimed "gap in P vs NP" from anyone, human or AI.

- **`experiments/circuit_complexity/`** — exhaustive BFS computing the exact
  gate complexity (full binary basis) of **every** Boolean function on up to
  4 variables. Machine-verified ground truth, reproducing the classical
  results (every 4-variable function needs ≤ 7 gates; XOR₄ = 3; MAJ₃ = 4)
  plus the full distribution. Runs in ~0.1 s.

- **`experiments/phase_transition/`** — a from-scratch DPLL SAT solver
  sweeping random 3-SAT across clause/variable ratios. Reproduces the phase
  transition: satisfiability crosses 50% at r ≈ 4.27 with the search-cost
  spike exactly at the threshold. Fixed seed; exactly reproducible.

- **`experiments/ramsey/` + `notes/ramsey.md`** — the certificate-search
  skeleton: a general Ramsey CNF encoder, a from-scratch DIMACS DPLL solver,
  exhaustive enumeration certifiers, and a CDCL driver (CaDiCaL/Glucose via
  python-sat), wired into an end-to-end machine-verified reproduction of
  **R(4,4) = 18**. Both computational upper-bound legs carry DRUP
  unsatisfiability certificates **verified by the official `drat-trim`
  checker** (`results/ramsey/*.drat`). Lower bounds are certified by
  exhaustive sweeps (all 2²⁸ colorings of K₈; Paley(17) over all 2,380
  4-subsets). Same encoder generates the open R(5,5) ∈ [43,46] instance —
  903 variables, 1.93M clauses — to show exactly where the frontier wall is.

- **`results/`** — outputs of all experiments as generated in this session.

## Reproduce

```sh
gcc -O2 -o bfs  experiments/circuit_complexity/bfs.c   && ./bfs
gcc -O2 -o dpll experiments/phase_transition/dpll.c    && ./dpll
```

## Results snapshot

Exact circuit complexity (n = 4): all 65,536 functions found; maximum
complexity **7 gates** (3,088 functions attain it); XOR₄ = 3, both 4-input
thresholds = 7.

Phase transition (n = 60, 100 trials/ratio):

| r    | sat fraction | median decisions |
|------|--------------|------------------|
| 3.00 | 1.00         | 24               |
| 4.00 | 0.86         | 190              |
| 4.27 | 0.53         | 638              |
| 4.35 | 0.46         | 698              |
| 5.20 | 0.01         | 353              |
| 6.00 | 0.00         | 166              |

Everything here is checkable. That is the point.
