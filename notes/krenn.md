# Krenn–Gu conjecture: exact tools and a certified exhaustive sweep at n = 6

Target: Mario Krenn's graph-theory question (bounty problem; page blocked
from this environment — definitions reconstructed from the literature and
corroborated via search; **cross-check definitions against the page and
arXiv:2407.00303 before external use**).

## The problem

An edge of a graph carries a complex weight and an ordered color pair
(one color shown at each endpoint; monochromatic = same both ends). A
perfect matching induces a vertex coloring (each vertex sees its matching
edge's color at that endpoint); the weight of a coloring is the sum over
matchings inducing it of the product of edge weights. G is a **GHZ graph
of dimension d** if all d monochromatic colorings have one common nonzero
weight and every mixed coloring has weight zero.
**Krenn–Gu conjecture:** more than 4 vertices forces d ≤ 2.
Known (Chandran–Gajjala et al.): true for vertex connectivity ≤ 2 and for
cubic graphs; a minimal counterexample is 4-connected. Known (Bogdanov):
true for positive real weights. Open in general; n = 6, d = 3 with
bichromatic edges and complex weights is the smallest wild territory.

## Tools built (`experiments/krenn/`)

- `verifier.py` — exact checker for any candidate (multigraph,
  bichromatic edges, Gaussian-rational weights). Sanity-validated on the
  three instructive cases: the K₄ d=3 GHZ graph (why n > 4 is required),
  the hexagon d=2, and the naive K₆ three-matching attempt, which fails
  with exactly three uncancellable mixed classes — the obstruction the
  conjecture generalizes.
- `gen_instance.py` + `sweep.c` — exhaustive filter over all
  monochromatic edge d-colorings of a support, enforcing two exact
  necessary conditions:
  **P1** every color class contains a perfect matching (else that
  monochromatic weight is an empty sum, i.e. 0 ≠ s);
  **P2** every realized mixed coloring class contains ≥ 2 matchings
  (a singleton class has weight = a product of nonzero weights ≠ 0).

## The sweep and what it proves

Scope: n = 6, simple support, monochromatic edges, nonzero weights
(zero-weight edges are deleted WLOG — the support is what remains).

Reduction chain: any 6-vertex counterexample is vertex-minimal (odd n
has no perfect matchings; n = 4 is not a counterexample), hence
4-connected by Chandran–Gajjala; a 4-connected graph on 6 vertices has
minimum degree ≥ 4, so its complement is a matching: the support is one
of exactly four graphs — K₆, K₆−M₁, K₆−M₂, K₆−M₃ (the octahedron).
Edge count caps the dimension: d disjoint monochromatic matchings need
3d ≤ 15, so d ≤ 5.

Result of the exhaustive sweep over every edge coloring of every
support (d = 3: 21.3M colorings; d = 4 and d = 5: see
`results/krenn_sweep.txt`): **every single coloring fails P1 or P2.**

**Machine-verified statement.** *No 6-vertex graph with simple support
and monochromatic edges is a GHZ graph of dimension ≥ 3, for any complex
edge weights* (modulo the imported 4-connectivity theorem; a
sweep of all 6-vertex supports would remove even that dependency).

Honest placement: the monochromatic-edge case of the conjecture may
well be known — the community regards bichromatic edges as the hard
part, and Bogdanov-type arguments may cover this slice. The value here
is the certified, reproducible, assumption-explicit form, and the
infrastructure. No counterexample was found (nor expected: the
open-territory prior is set by years of targeted search, including by
Krenn's own automated-discovery tools).

## Roadmap toward the open territory

The genuinely open zone (bichromatic edges, complex weights) admits the
same program in slices of increasing bichromatic budget k: the P1/P2
filter generalizes verbatim (bichromatic edges never serve in
monochromatic matchings; singleton mixed classes remain fatal), with an
exact algebraic stage (Gröbner over ℚ(i), Rabinowitsch trick for the
nonzero conditions) deciding the survivors. k = 1 on all four supports
is a feasible next compute (~10⁷·d² colorings); the full space is where
the bounty lives and where naive enumeration dies — any real attack
needs symmetry reduction and the algebraic stage doing heavy lifting.

## Reproduce

```sh
cd experiments/krenn
python3 verifier.py                       # sanity trio
for s in octahedron K6-M2 K6-M1 K6; do
  python3 gen_instance.py $s | grep -v "^c" > inst_$s.dat
done
gcc -O2 -o sweep sweep.c
./sweep inst_K6.dat 3                     # etc.: supports x d in {3,4,5}
```
