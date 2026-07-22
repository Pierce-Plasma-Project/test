# Rado certificates

Each value R_3(ax+by=cz) = n has: the UNSAT CNF at n (`.cnf`) and a
Glucose-3 DRUP proof (`.drat`), verified with drat-trim (`s VERIFIED`).

CNFs over git's size limit are not committed; they regenerate
deterministically:

    python3 experiments/rado/rado.py  # module: clause_stream(a,b,c,n)

- `r3_1_26_26_n17576.cnf` (685 MB, not committed):
  sha256 = 9a5a967a54b575091563ee35fd4b01b36a1e8f74b95cc0383edc7b77f63c6330
  regenerate: clause_stream(1, 26, 26, 17576) -> DIMACS
  Its 17 KB DRAT proof IS committed and verifies against the
  regenerated CNF: drat-trim <cnf> r3_1_26_26_n17576.drat

- `r3_16_25_25_n15625.cnf` (444 MB, not committed):
  sha256 = d31a3dd390edcf563741ff0b7f3de5727a790ca7f31868ec8f62b36385ad256b
  regenerate: clause_stream(16, 25, 25, 15625) -> DIMACS
  committed alongside: its 300 KB DRAT proof (drat-trim VERIFIED) and the
  gzipped extremal 3-coloring of [1,15624] (witness for the lower bound;
  independently checked before archiving).

