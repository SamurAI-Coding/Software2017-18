#!/usr/bin/env python3
from __future__ import print_function
from builtins import input
import numpy as np
import sys
import itertools
import random
import math

from py3_map import Map


def readline():
    x = input()
    print(str(x), file=sys.stderr)
    return x

def next_state(p, ac, m):
    next_v = p[1] + ac
    next_p = p[0] + next_v
    if m.has_collision(p[0], next_p):
        return (p[0], next_v)
    return (next_p, next_v)


def evaluate(ps, m):
    return ps[0][1]


def main():                                
    total_time = int(readline())
    max_step = int(readline())
    width, height = [int(x) for x in readline().split()]
    view = int(readline())
    print(0)
    m = Map(width, height)
    while True:
        step = int(readline())
        time = int(readline())
        ps = []
        for j in range(2):
            xs = [int(x) for x in readline().split()]
            p = [np.array([xs[0], xs[1]]), np.array([xs[2], xs[3]])]
            ps.append(tuple(p))
        for y in range(ps[0][0][1] - view, ps[0][0][1] + view + 1, 1):
            ls = [int(v) for v in readline().split()]
            if y > 0:
                m.setline(y, ls)
        moves = []
        for ax, ay in itertools.product(range(-1, 2), range(-1, 2)):
            next_ps = next_state(ps[0], np.array([ax, ay]), m)
            if (next_ps[0] == ps[0][0]).all() or next_ps[0][1] < ps[0][0][1]:
                continue
            moves.append((ax, ay))
        if len(moves) > 0:
            acs = moves[random.randrange(0, len(moves))]
        else:
            acs = (-np.sign(ps[0][1][0]), -np.sign(ps[0][1][1]))
        cmd = ' '.join(str(x) for x in acs)
        print('cmd = %s' % cmd, file=sys.stderr)
        print(cmd)

if __name__ == '__main__':
    main()
