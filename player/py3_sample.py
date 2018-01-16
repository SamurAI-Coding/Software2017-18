#!/usr/bin/env python3
from __future__ import print_function
from builtins import input
import numpy as np
import sys
import itertools
import math

from py3_map import Map

def make_cost_map(m, miny):
    cost_m = Map(m.w, m.h, 1000, 1000)
    x = -1
    maxy = min(m.maxy, m.h)
    while x <= m.w:
        x += 1
        if m.getXY(x - 1, maxy) > 0 and m.getXY(x, maxy) == 0:
            x1 = x + 1
            while m.getXY(x1, maxy) == 0:
                x1 += 1
            for x2 in range(x, x1, 1):
                cost_m.setXY(x2, maxy, m.w - min(x2 - x, x1 - x2 - 1))
    for y in range(maxy - 1, max(0, miny) - 1, -1):
        for x in range(m.w):
            if m.getXY(x, y) == 0:
                cost_m.setXY(x, y, cost_m.getXY(x, y + 1) + 5)
        for x in range(m.w):
            if m.getXY(x, y) == 0:
                v = min(cost_m.getXY(x, y), cost_m.getXY(x - 1, y) + 1)
                cost_m.setXY(x, y, v)
        for x in range(m.w - 1, -1, -1):
            if m.getXY(x, y) == 0:
                v = min(cost_m.getXY(x, y), cost_m.getXY(x + 1, y) + 1)
                cost_m.setXY(x, y, v)
    return cost_m


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


def evaluate(ps, m, cost_m):
    cost = -cost_m.getP(ps[0])
    return cost - 5 * np.sum(np.absolute(ps[1]))


def search(pos, m, cost_m, depth):
    if depth == 0:
        return evaluate(pos, m, cost_m)
    maxy = min(m.maxy, m.h)
    if pos[0][1] + pos[1][1] >= maxy:
        return evaluate(pos, m, cost_m) + depth * 100
    maxv = float('-inf')
    for ax, ay in itertools.product(range(-1, 2), range(-1, 2)):
        next_pos = next_state(pos, np.array([ax, ay]), m)
        v = search(next_pos, m, cost_m, depth - 1)
        maxv = max(maxv, v)
    return maxv


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
        acs, maxv = None, float('-inf')
        cost_m = make_cost_map(m, ps[0][0][1] - view)
        for ax, ay in itertools.product(range(-1, 2), range(-1, 2)):
            next_pos = next_state(ps[0], np.array([ax, ay]), m)
            if (next_pos[0] == ps[0][0]).all():
                continue
            v = search(next_pos, m, cost_m, 3)
            if v > maxv:
                maxv = v
                acs = (ax, ay)
        if maxv < -1000000.0:
            vv = max(np.absolute(ps[0][1]))
            for ax, ay in itertools.product(range(-vv, vv + 1), range(-vv, vv + 1)):
                next_pos = next_state(ps[0], np.array([ax, ay]), m)
                if (next_pos[0] == ps[0][0]).all():
                    continue
                v = max(abs(ax), abs(ay)) * -100 + evaluate(next_pos, m, cost_m)
                if v > maxv:
                    maxv = v
                    acs = (np.sign(ax), np.sign(ay))
        if acs is None:
            acs = (-np.sign(ps[0][1][0]), -np.sign(ps[0][1][1]))
        cmd = ' '.join(str(x) for x in acs)
        print(cmd)

if __name__ == '__main__':
    main()
