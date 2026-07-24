#!/usr/bin/env python3
"""Continuous optimization over C for GHZ graphs (the PyTheus-style method).

Fundamentally different from the finite-field/discrete searches: amplitudes
are complex numbers, so we minimize the sum of squared constraint
violations with a real gradient-based least-squares solver (Levenberg-
Marquardt via scipy), from many random complex restarts.

Variables: for n=8, d=3, every vertex pair carries a 3x3 complex amplitude
matrix (subsumes multigraphs + bichromatic edges). Residuals:
  - each mixed vertex-coloring class weight = 0   (real+imag)
  - each mono class weight = 1                     (real+imag; pins scale)
A run reaching ~0 residual is a numerical GHZ graph of dimension 3 on 8
vertices => a Krenn-Gu counterexample. It is then rationalized and handed
to the exact verifier before any claim.

Usage: optimize.py N [seed_lo seed_hi] [--ansatz cc|pf]
"""
import itertools
import sys

import numpy as np
from scipy.optimize import least_squares

N = int(sys.argv[1]) if len(sys.argv) > 1 else 8
D = 3
SEED_LO = int(sys.argv[2]) if len(sys.argv) > 2 else 0
SEED_HI = int(sys.argv[3]) if len(sys.argv) > 3 else 40
ANSATZ = "pf"
if "--ansatz" in sys.argv:
    ANSATZ = sys.argv[sys.argv.index("--ansatz") + 1]


def perfect_matchings(n):
    out = []
    def rec(un, ch):
        if not un:
            out.append(tuple(ch)); return
        v0 = min(un)
        for x in sorted(un - {v0}):
            rec(un - {v0, x}, ch + [(v0, x)])
    rec(frozenset(range(n)), [])
    return out


PMS = perfect_matchings(N)

# variable layout
if ANSATZ == "pf":
    pairs = {}
    for u in range(N):
        for v in range(u + 1, N):
            pairs[(u, v)] = len(pairs)
    NV = len(pairs) * D * D
    def vidx(u, v, a, b):
        if u > v:
            u, v, a, b = v, u, b, a
        return pairs[(u, v)] * D * D + a * D + b
elif ANSATZ == "cc":                       # circulant color-cyclic
    nd = N // 2
    NV = nd * D
    def vidx(u, v, a, b):
        dd = min(abs(u - v), N - abs(u - v)) - 1
        return dd * D + ((b - a) % D)
else:
    raise SystemExit("ansatz cc|pf")

# precompute, per class, the list of matchings as arrays of variable indices
CLASSES = []          # (is_mono, [ [vidx,...] per matching ])
for kappa in itertools.product(range(D), repeat=N):
    mats = []
    for pm in PMS:
        mats.append([vidx(u, v, kappa[u], kappa[v]) for (u, v) in pm])
    CLASSES.append((len(set(kappa)) == 1, np.array(mats, dtype=np.int64)))

MONO = np.array([i for i, (m, _) in enumerate(CLASSES) if m])
MIX = np.array([i for i, (m, _) in enumerate(CLASSES) if not m])
# stacked index tensor: (NC, NPM, half) — every class, every matching, every edge
IDX = np.stack([c[1] for c in CLASSES]).astype(np.int64)   # (NC, NPM, half)


def weights(z):
    """All class weights at once: prod over edges, sum over matchings."""
    return np.prod(z[IDX], axis=2).sum(axis=1)              # (NC,)


def residuals(x):
    z = x[:NV] + 1j * x[NV:]
    w = weights(z)
    wm = w[MIX]
    wc = w[MONO] - 1.0                                       # pin monos to 1
    return np.concatenate([wm.real, wm.imag, wc.real, wc.imag])


def run():
    print(f"n={N} d={D} ansatz={ANSATZ} NV={NV} classes={len(CLASSES)} "
          f"(mono {len(MONO)}, mixed {len(MIX)}) PMs={len(PMS)}", flush=True)
    best = np.inf
    logf = open(f"optlog_{ANSATZ}_{N}_{SEED_LO}.txt", "w")
    for seed in range(SEED_LO, SEED_HI):
        r = np.random.default_rng(seed)
        x0 = r.standard_normal(2 * NV) * 0.5
        sol = least_squares(residuals, x0, method="lm", max_nfev=300)
        c = 2 * sol.cost
        if c < best:
            best = c
            logf.write(f"seed {seed}: residual^2 = {c:.3e}\n"); logf.flush()
        if c < 1e-16:
            z = sol.x[:NV] + 1j * sol.x[NV:]
            print("SOLUTION (numerical): residual", c)
            np.save(f"ghz_sol_n{N}_{seed}.npy", z)
            print("saved; rationalize + exact-verify next")
            return z
    logf.write(f"DONE best residual^2 = {best:.3e}\n"); logf.flush(); logf.close()
    print(f"no numerical GHZ graph found; best residual^2 = {best:.3e}")
    return None


if __name__ == "__main__":
    run()
