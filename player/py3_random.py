#!/usr/bin/env python
from __future__ import print_function
from builtins import input
import numpy as np
import sys
import itertools
import random
import math


class Map:
    def __init__(self, width, height, view):
        self.w = width
        self.h = height
        self.m = [[0] * width for y in range(height + view + 1)]
        self.maxy = 0

    def setline(self, y, l):
        self.m[y] = l
        self.maxy = max(self.maxy, y)

    def getXY(self, x, y):
        if x < 0 or x >= self.w or y < 0 or y > self.maxy:
            return 1
        return self.m[y][x]

    def getP(self, p):
        return self.getXY(p[0], p[1])


def readline():
    x = input()
    print(str(x), file=sys.stderr)
    return x


def has_collision(p, next_p, m, view):
    if m.getP(p) > 0 or m.getP(next_p) > 0:
        return True
    dp = next_p - p
    xlen, ylen = abs(dp[0]), abs(dp[1])
    dx, dy = np.sign(dp[0]), np.sign(dp[1])
    for i in range(1, xlen + 1, 1):
        x = int(p[0] + dx * i)
        y = p[1] + (ylen * dy * i) / xlen
        y0 = int(math.floor(y))
        y1 = int(math.ceil(y))
        if m.getXY(x, y0) > 0:
            for k in range(-1, 2, 1):
                if m.getXY(x + k, y1) > 0:
                    return True
        if m.getXY(x, y1) > 0:
            for k in range(-1, 2, 1):
                if m.getXY(x + k, y0) > 0:
                    return True
    for i in range(1, ylen + 1, 1):
        y = int(p[1] + dy * i)
        x = p[0] + (xlen * dx * i) / ylen
        x0 = int(math.floor(x))
        x1 = int(math.ceil(x))
        if m.getXY(x0, y) > 0:
            for k in range(-1, 2, 1):
                if m.getXY(x1, y + k) > 0:
                    return True
        if m.getXY(x1, y) > 0:
            for k in range(-1, 2, 1):
                if m.getXY(x0, y + k) > 0:
                    return True
    return False


def next_state(p, ac, m, view):
    next_v = p[1] + ac
    next_p = p[0] + next_v
    if has_collision(p[0], next_p, m, view):
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
    m = Map(width, height, view)
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
            next_ps = next_state(ps[0], np.array([ax, ay]), m, view)
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
