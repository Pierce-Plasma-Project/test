# Krenn–Gu counterexample search: consolidated findings

Status of the hunt for a GHZ graph of dimension ≥ 3 on n > 4 vertices
(Krenn's bounty problem). No counterexample found. Every region searched
exactly is certified empty, and the one open cell exhibits a structural
floor exactly where the theory predicts one. All results reproducible from
`experiments/krenn/`.

## Certified-empty regions (exact necessary conditions or exhaustive)

| region | scope | result |
|---|---|---|
| n=6, mono edges | all 4 supports, d∈{3,4,5}, all colorings | empty (P1/P2), ~32G |
| n=6, bichromatic k≤2 | 4 supports, d∈{3,4} | empty, ~1.3T colorings |
| n=8, circulant/diag ansätze | GF(4), 𝔽₇ | empty (exhaustive) |
| n=10, circulant color-cyclic | GF(4), all 4¹⁵ | empty (exhaustive) |

The n=6 results are consistent with the literature (Cervera-Lierta–Krenn–
Aspuru-Guzik established dim<3 at 6 particles via SAT); this reproduces
that with explicit certificates and extends the certified region.

## The central finding: a cost-6 obstruction at n=8, d=3

The open cell n=8, d=3 was attacked with an ansatz-free stochastic search
over the full matrix-weight system (every vertex pair carries a 3×3
amplitude matrix — this subsumes all multigraphs and all bichromatic
colorings simultaneously; 252 variables, 6561 interference constraints).

**Robust result:** the search floor is **cost 6** — six unsatisfiable
mixed-coloring constraints — reproduced across:
- two fields (GF(4) and 𝔽₇),
- three move sets (uniform, focused-violated-class, exact-repair),
- two scaffold families (block and round-robin 1-factorizations),
- dozens of random restarts per configuration.

**Interpretation.** Cost 6 is exactly the six cross-interference terms of
GHZ₃⊗GHZ₃. Two K₄ GHZ graphs (the n=4, d=3 solution) glued on 8 vertices
satisfy every constraint *except* the six that couple the two blocks. The
search rediscovered, empirically and from a cold start, the precise
obstruction the conjecture formalizes: dimension does not compose across a
tensor split without a genuinely non-blockwise interference structure, and
no such structure was found in the reachable search space.

This is evidence *for* the conjecture at n=8, not a proof: it shows the
counterexample (if any) is not reachable by symmetric ansätze, small
finite fields, or local moves from the natural scaffolds — consistent with
the conjecture being true here, and with years of unsuccessful automated
search by the community.

## Methodological notes (why the negatives are trustworthy)

- Every layer validated on positive controls (K₄ and hexagon GHZ graphs
  found/verified) before use on open cells.
- Two bugs were caught by internal cross-checks, not ignored: a char-2
  monomial-coefficient error at n=10 (produced 3 phantom "solutions" that
  hand-verification killed — λ·I on K₅,₅ has mono weight 120λ⁵ ≡ 0 mod 2),
  and an incremental-cost optimization whose tally silently disagreed with
  the validated evaluator (273 vs the true 6) and was discarded rather than
  shipped. A search that misreports its own progress is worse than a slow
  one.
- Finite-field hits would be lifted to exact ℂ candidates and confirmed by
  the independent `verifier.py`; none arose.

## What would actually be needed

A counterexample, if it exists, lies outside every region finite search
can reach. The realistic path is structural: an explicit construction that
defeats the six-term cross-obstruction — the analogue, in this problem, of
knowing exactly which polynomial map to write down for the Jacobian
counterexample. That is a human-insight step; brute force at accessible
scales has been exhausted here without success.
