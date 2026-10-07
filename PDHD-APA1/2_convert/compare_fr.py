#!/usr/bin/env python3
'''
Compare two WCT field response files path by path.

Usage: python3 compare_fr.py a.json.bz2 b.json.bz2

Prints every path whose current differs by more than 1e-6 relative to
its peak, then the number of differing paths.
'''
import sys, json, bz2
import numpy as np


def load(fname):
    fr = json.load(bz2.open(fname))['FieldResponse']
    meta = {k: v for k, v in fr.items() if k != 'planes'}
    paths = {}
    for plane in fr['planes']:
        pr = plane['PlaneResponse']
        for path in pr['paths']:
            p = path['PathResponse']
            paths[(pr['planeid'], round(p['pitchpos'], 3))] = np.array(p['current']['array']['elements'])
    return meta, paths


ma, a = load(sys.argv[1])
mb, b = load(sys.argv[2])
print('metadata', 'same' if ma == mb else f'DIFFERENT: {ma} vs {mb}')
ndiff = 0
worst = 0.0
for key in sorted(set(a) | set(b)):
    if key not in a or key not in b:
        print('only in one file:', key)
        ndiff += 1
        continue
    x, y = a[key], b[key]
    if x.shape != y.shape:
        print(key, 'shape', x.shape, y.shape)
        ndiff += 1
        continue
    rel = np.max(np.abs(x - y)) / (np.max(np.abs(x)) or 1.0)
    worst = max(worst, rel)
    if rel > 1e-6:
        print(f'plane {key[0]} pitchpos {key[1]:8.3f} mm  max rel diff {rel:.3g}')
        ndiff += 1
print(f'{len(a)} paths, {ndiff} differ, worst relative difference {worst:.3g}')
