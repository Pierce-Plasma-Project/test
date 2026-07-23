#!/usr/bin/env python3
"""Generate sweep instance files for the four 4-connected 6-vertex
supports (the only simple-graph candidates for a minimal Krenn-Gu
counterexample by Chandran-Gajjala 4-connectivity, which requires
minimum degree >= 4 on 6 vertices).

Instance file format (consumed by sweep.c):
  line 1: E  nPM  n
  per PM: 2n ints — e_1..e_n/2 (edge indices, n/2 of them) then the
          vertex->edge map v_0..v_{n-1} (edge index matched at vertex v)
"""
import itertools
import sys

N = 6


def edges_of(support):
    full = [(u, v) for u in range(N) for v in range(u + 1, N)]
    if support == "K6":
        drop = []
    elif support == "K6-M1":
        drop = [(0, 1)]
    elif support == "K6-M2":
        drop = [(0, 1), (2, 3)]
    elif support == "octahedron":       # K6 minus a perfect matching
        drop = [(0, 1), (2, 3), (4, 5)]
    else:
        raise SystemExit(f"unknown support {support}")
    return [e for e in full if e not in drop]


def perfect_matchings(edges):
    out = []
    def rec(unmatched, chosen):
        if not unmatched:
            out.append(list(chosen))
            return
        v0 = min(unmatched)
        for i, (u, v) in enumerate(edges):
            if v0 in (u, v):
                o = v if u == v0 else u
                if o in unmatched and o != v0:
                    rec(unmatched - {u, v}, chosen + [i])
    rec(frozenset(range(N)), [])
    return out


def main():
    support = sys.argv[1]
    edges = edges_of(support)
    pms = perfect_matchings(edges)
    print(f"{support}: {len(edges)} edges, {len(pms)} perfect matchings",
          file=sys.stderr)
    print(len(edges), len(pms), N)
    for pm in pms:
        v2e = [None] * N
        for i in pm:
            u, v = edges[i]
            v2e[u] = v2e[v] = i
        print(" ".join(map(str, pm + v2e)))
    # edge list appended as comments for the analysis stage
    for u, v in edges:
        print(f"c edge {u} {v}")


if __name__ == "__main__":
    main()
