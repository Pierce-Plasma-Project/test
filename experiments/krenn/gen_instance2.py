#!/usr/bin/env python3
"""Extended instance files for the bichromatic sweep (sweep2.c).

Format:
  line 1: E  nPM  N
  per PM: n/2 edge indices, then N vertex->edge, then N vertex->end
          (0 if the vertex is the u-endpoint of its matching edge, 1 if v)
  then E lines: "e u v" (the edge list, for the algebraic stage)
"""
import sys
from gen_instance import edges_of, perfect_matchings, N


def main():
    support = sys.argv[1]
    edges = edges_of(support)
    pms = perfect_matchings(edges)
    print(f"{support}: {len(edges)} edges, {len(pms)} PMs", file=sys.stderr)
    print(len(edges), len(pms), N)
    for pm in pms:
        v2e = [None] * N
        v2end = [None] * N
        for i in pm:
            u, v = edges[i]
            v2e[u] = v2e[v] = i
            v2end[u], v2end[v] = 0, 1
        print(" ".join(map(str, list(pm) + v2e + v2end)))
    for i, (u, v) in enumerate(edges):
        print(f"e {u} {v}")


if __name__ == "__main__":
    main()
