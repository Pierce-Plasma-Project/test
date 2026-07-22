# P vs NP: the actual frontier, and where the gaps are

**Epistemic status.** Nothing in this document is a novel result, and it claims
none. Every statement is either (a) an established theorem with attribution,
(b) an explicitly-marked paraphrase of one, or (c) clearly labeled commentary.
That is deliberate: on this problem, the difference between *progress* and
*text that resembles progress* is verifiability, and this document is built to
be checkable line by line. As of mid-2026 there is no published, peer-verified
advance that breaks the barrier structure described below — by any lab or any
model. Anyone claiming otherwise, human or AI, owes the community a checkable
artifact; Section 8 is the checklist such an artifact must survive.

---

## 1. Why the problem is hard *in a provable sense*

P vs NP is unusual among open problems: we have **theorems about why our
proof techniques cannot work**. Three barriers, each a real theorem:

**Relativization** (Baker–Gill–Solovay 1975). There exist oracles A and B with
P^A = NP^A and P^B ≠ NP^B. Consequence: any proof whose every step remains
valid when all machines are given the same oracle — which covers essentially
all of classical computability-style technique (simulation, diagonalization,
padding) — cannot resolve P vs NP in either direction.

**Natural proofs** (Razborov–Rudich 1994). Call a property of Boolean
functions *natural* if it is (i) constructive — decidable in time polynomial
in the truth-table size — and (ii) large — holds for a non-negligible fraction
of all functions. Theorem (paraphrase): a natural property that suffices to
prove super-polynomial circuit lower bounds against P/poly would distinguish
pseudorandom functions from random ones, and hence would break exponentially
hard pseudorandom function families. If such PRFs exist (they follow from
standard cryptographic assumptions, e.g. hardness of factoring), then **no
natural proof shows NP ⊄ P/poly**. Nearly every combinatorial lower-bound
argument known before 1994 — and most after — is natural.

**Algebrization** (Aaronson–Wigderson 2008). The interactive-proof techniques
that famously *do* evade relativization (arithmetization, as in IP = PSPACE)
are captured by a refined barrier: proofs that remain valid when machines may
query low-degree algebraic extensions of an oracle. P vs NP (again, in either
direction) does not algebrize. So the one great post-1975 technique family is
also provably insufficient on its own.

Commentary: these do not say the problem is unsolvable. They say a solution
must contain a *technique* outside three well-mapped classes — which is
exactly why credible progress is identifiable: you can ask of any claimed
advance, "which barrier does it evade, and at which step?" A claim that cannot
answer this question is not at the frontier; it is behind it.

## 2. Where lower bounds actually stand (the scoreboard)

The statement NP ⊄ P/poly (circuit version of P ≠ NP, and the standard route
to it) requires super-polynomial circuit lower bounds for some NP function.
What is actually proven, for explicit functions:

- **General circuits (basis B2):** the record is **linear**: 3n − o(n)
  (Blum 1984), stuck for three decades, then (3 + 1/86)n (Find–Golovnev–
  Hirsch–Kulikov 2016), then ≈ 3.1n (Li–Yang 2022). We cannot prove that any
  NP function needs 4n gates, let alone n·log n, let alone super-polynomial.
  The distance between "3.1n" and "super-polynomial" is the honest measure of
  the gap.
- **De Morgan formulas:** n^{3−o(1)} (Håstad 1998, via shrinkage; refined by
  Tal). Cubic, stuck since the 1990s.
- **Constant depth, AND/OR (AC⁰):** exponential (Håstad 1986). Solved regime.
- **AC⁰ with mod-p gates, p prime:** exponential-type bounds
  (Razborov 1987, Smolensky 1987). Solved regime — and the techniques are
  *natural*, which is precisely why they stop here.
- **AC⁰ with mod-6 gates (ACC⁰):** no exponential bounds for NP; what exists
  is Williams 2011: **NEXP ⊄ ACC⁰**, strengthened to NQP ⊄ ACC⁰
  (Murray–Williams 2018) and to average-case / almost-everywhere variants
  (Chen; Chen–Ren; Chen–Lyu–Williams 2019–2021). This is the celebrated
  *algorithmic method* result — see §3.
- **Threshold circuits (TC⁰):** for unrestricted weights and depth as small
  as **three**, no super-polynomial lower bound is known for any NP function.
  This is the wall where "neural-net-shaped" circuit classes begin.
- **Monotone circuits:** exponential (Razborov 1985; Alon–Boppana 1987).
  Solved regime, and provably non-transferable to general circuits by known
  routes (monotone vs general gaps are exponential).
- **Near-maximum bounds, big classes:** S₂E requires circuits of size
  2ⁿ/n-type near-maximum (Chen–Hirahara–Ren 2023, dramatically simplified and
  strengthened by Li 2024, via the range-avoidance program). Genuine recent
  movement — for a class far above NP.

Reading the scoreboard honestly: every "solved regime" was solved by a
technique that provably dies at the next regime (natural-proofs barrier for
Razborov–Smolensky; relativization/algebrization elsewhere). The unsolved
regimes are unsolved *for identified reasons*.

## 3. The live programs, and the precise gap in each

### 3.1 The algorithmic method (Williams)

**Engine (theorem, paraphrase):** a Circuit-SAT algorithm for circuit class C
running even slightly faster than brute force (2ⁿ/n^{ω(1)}) implies
NEXP ⊄ C. Hardness flows *from* algorithms: this evades relativization and
natural proofs because the lower bound is powered by a non-black-box
algorithmic insight, not a combinatorial property of random functions.

**Delivered:** NEXP ⊄ ACC⁰ (2011) via a nontrivial ACC⁰-SAT algorithm built
on the Yao–Beigel–Tarui representation of ACC⁰ by quasi-polynomial-degree
polynomials.

**The gap, precisely:** to advance to TC⁰ or NC¹ or P/poly, one needs
better-than-brute-force SAT algorithms for those classes. None are known, and
there is structural pushback: such algorithms for strong enough classes come
close to refuting the Strong Exponential Time Hypothesis, and known
representation theorems (the Beigel–Tarui step) have no TC⁰ analogue. Also,
the method as constituted lands at NEXP/NQP, not NP; bringing the hard
function down to NP needs additional ideas ("witness compression" style
arguments have known limits).
**Missing lemma shape:** "Circuit-SAT for depth-d threshold circuits in time
2ⁿ/n^{ω(1)}" — or a replacement engine that does not route through SAT.

### 3.2 Hardness magnification + the locality barrier

**Phenomenon (OPS 2019; McKay–Murray–Williams 2019; paraphrase):** for
certain *meta-computational* problems — canonically Gap-MCSP[s], deciding
whether a length-N = 2ⁿ truth table has circuit complexity below s(n) or far
above — **weak** lower bounds magnify to **breakthrough** separations. A
representative form: if Gap-MCSP[2^{√n}] requires circuits (resp. formulas)
of size N^{1+ε} for some fixed ε > 0, then NP ⊄ P/poly (resp. NP ⊄ NC¹).
Lower bounds of that *magnitude* are routinely provable for related problems
in weaker settings — which is what makes the program tantalizing: the wall
looks centimeters thick here.

**The gap, precisely (the locality barrier, Chen–Hirahara–Oliveira–Pich–
Rajgopal–Santhanam 2020):** the magnification theorems work by *compressing*
the meta-problem: they show the target problem is solvable by small circuits
equipped with **local oracles** (oracle gates whose queries are short/local).
Hence the needed N^{1+ε} lower bound must *fail* to hold against circuits
with local oracles — i.e., it must be a fundamentally **non-localizing**
technique. Then the theorem: essentially all known lower-bound techniques in
the relevant size regimes (random restrictions, shrinkage, approximation by
polynomials, communication-complexity transfers…) **do localize** — they
prove the same bounds even against local-oracle circuits, so they can never
prove the magnification hypothesis. This is not a vague analogy barrier; it
is the sharpest known formalization of "so near and yet so far."
**Missing lemma shape:** an N^{1+ε} lower bound for a magnification target
whose proof breaks when the circuit is granted local oracles. No such
technique exists in the literature. If someone claims magnification-route
progress, this is the *first* thing to check.

### 3.3 Meta-complexity (Hirahara's program)

**Delivered (paraphrases):** worst-case-to-average-case connections for NP
routed through Gap-MCSP (Hirahara 2018); NP-hardness of increasingly close
relatives of MCSP — partial-function MCSP under randomized reductions
(Ilango 2020; Hirahara 2022), multi-output variants (Ilango–Loff–Oliveira) —
while NP-hardness of MCSP itself remains open and is itself barrier-laden
(deterministic NP-hardness of MCSP would already imply EXP ≠ ZPP-type
separations, i.e., the hardness proof is itself gated on lower bounds).

**The gap, precisely:** close the loop — NP-hardness of Gap-MCSP in the
parameter regime that composes with the worst-to-average machinery and with
magnification. Each pairwise composition is known; the triple composition is
exactly what does not exist.

### 3.4 Geometric complexity theory (GCT)

**Program:** resolve permanent vs determinant (VP vs VNP, the algebraic
cousin whose separation is *necessary-flavored* groundwork for P vs NP over
ℂ) via representation theory: find obstructions — irreducible representations
occurring in the coordinate ring of one orbit closure but not the other.

**The gap, precisely:** the cheap version is dead: **occurrence obstructions
do not exist** in the required regime (Bürgisser–Ikenmeyer–Panova 2016). What
remains must use *multiplicity* obstructions, and computing/lower-bounding
multiplicities (Kronecker coefficients etc.) is itself #P-hard-flavored
territory. Even the algebraic warm-up VP ≠ VNP remains open; the best
determinantal-complexity lower bound for the permanent is quadratic
(Mignon–Ressayre 2004). Distance to P ≠ NP: two conjectures deep.

### 3.5 Proof complexity (Cook's program)

NP ≠ coNP (which implies P ≠ NP) is equivalent to: no propositional proof
system admits polynomial-size proofs of all tautologies. Scoreboard:
resolution — exponential (Haken 1985, pigeonhole); bounded-depth Frege —
exponential (Ajtai 1988; Krajíček–Pudlák–Woods; Pitassi–Beame–Impagliazzo);
**AC⁰[p]-Frege — nothing super-polynomial, open since the late 1980s**;
Frege / extended Frege — nothing, and candidate hard tautologies are scarce.
The stall mirrors the circuit stall at exactly the "counting gates" frontier,
for related reasons (the Razborov–Smolensky toolkit does not lift).

## 4. The algorithms side (the other direction)

P = NP would also be "progress on the gap," so the honest map includes it:
best exact 3-SAT algorithms run in ≈ 1.307ⁿ (PPSZ line: Paturi–Pudlák–Saks–
Zane; Hertli 2011; refinements by Scheder et al.) — decades of work moved the
base from 2 to 1.307 with no hint of sub-exponential. The Exponential Time
Hypothesis (no 2^{o(n)} 3-SAT) and SETH formalize the empirical wall and now
underpin fine-grained complexity; a refutation of either would be legitimate
seismic progress. None exists.

## 5. What *verified* frontier movement looked like recently

Calibration for claims: the last two years produced genuine, peer-verified
movement *adjacent* to the problem — none of it touching the P vs NP barrier
structure itself:

- Tree Evaluation in O(log n · log log n) space (Cook–Mertz 2024).
- **TIME[t] ⊆ SPACE(≈√t)** (Williams 2025) built on it — the first
  fundamentally new time-space simulation in fifty years, with corollaries
  like SPACE[s] ⊄ TIME[s^{2−ε}]. This is what a real breakthrough looks like:
  a clean statement, a short proof, immediate community verification.
- Near-maximum circuit lower bounds for S₂E (Chen–Hirahara–Ren 2023, Li 2024).

Note the pattern: each is a *checkable artifact* that survived hostile
reading within weeks. That is the admission standard, and it is exactly the
standard "unpublished internal progress" has not yet met — from anyone.

## 6. Quantified belief, for honesty

Community consensus (e.g., recurring polls of complexity theorists) sits near
P ≠ NP at ~85–97%, with essentially no expectation of resolution soon.
Scott Aaronson's summary remains the fairest one-line status: we understand
*why* the problem is hard with unusual precision, and that understanding is
itself the field's main product so far.

## 7. What this repository's experiments are

Two small, fully-reproducible computations live under `experiments/`,
results under `results/`:

1. **Exact circuit complexity, exhaustively** (`circuit_complexity/`):
   BFS over all truth tables computes the exact B2 gate complexity of *every*
   Boolean function on ≤ 4 variables. Output reproduces classical ground
   truth (max complexity 7 at n = 4 — cf. Knuth TAOCP §7.1.2; XOR₄ = 3;
   MAJ₃ = 4) and exhibits the exact distribution. This is the n at which
   *exhaustive certainty* is feasible; the entire lower-bound problem of §2
   is that nothing resembling this scales — the search space is doubly
   exponential, which is why explicit lower bounds sit at 3.1n.
2. **The 3-SAT phase transition** (`phase_transition/`): a from-scratch DPLL
   solver run on random 3-SAT across clause/variable ratios, reproducing the
   empirical hardness spike at the satisfiability threshold r ≈ 4.27 — the
   empirical face of NP-hardness.

Neither is new science; both are *true*, checkable, and regenerate from
source in seconds with fixed seeds. That is the currency this problem trades
in.

## 8. Rubric: how to audit any claimed "gap in P vs NP"

Apply in order; most claims die at (1)–(3).

1. **Barrier accounting.** Which of relativization / natural proofs /
   algebrization does the argument evade, and at *which numbered step*? If
   the author cannot point to the step, the proof relativizes and is wrong.
2. **Locality check** (for magnification-route claims). Does the claimed
   lower bound survive granting the circuit local oracles? If yes, it cannot
   feed magnification (§3.2). If no, identify the non-localizing step — that
   step alone would be a publishable paper.
3. **Regime sanity.** Does the technique, run at weaker parameters, prove
   something already known to be *false* or already known by easier means at
   a suspiciously discounted price (e.g., would it also give TC⁰ bounds
   nobody has)? Overshoot is the classic failure mode of flawed proofs.
4. **Explicitness.** Is the hard function actually in NP, with the reduction
   fully specified? (Half of historical false proofs quantify over a
   non-constructive function.)
5. **The P = NP side.** If it is an algorithm: does it beat 1.307ⁿ on random
   3-SAT at r = 4.27 *in code*? Benchmarks or it did not happen — this one is
   empirically falsifiable in an afternoon, e.g. with `experiments/`-style
   harnesses.
6. **Formalizability.** Will the authors commit the argument to Lean/Coq
   scaffolding, or at least to a step-numbered version amenable to hostile
   review? Refusal is diagnostic.
7. **Provenance.** "Unpublished, internal, can't show you" has, historically,
   a 100% base rate of not surviving contact with items (1)–(6). Priors
   should be set accordingly — for any lab.

---

*References are cited by author-year in text; all are standard and locatable
by title in any search engine. Paraphrases are marked as such; consult the
original papers for exact statements before building on them.*
