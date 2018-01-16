#!/usr/bin/env python3

def cross(x1, y1, x2, y2):
    return x1 * y2 - x2 * y1

def dot(x1, y1, x2, y2):
    return x1 * x2 + y1 * y2

def ccw(x1, y1, x2, y2, x3, y3):
    return cross(x2 - x1, y2 - y1, x3 - x2, y3 - y2)

class LineSegment:
    def __init__(self, p0, p1):
        self.p0 = p0
        self.p1 = p1

    def intersects(self, s):
        return self.both_sides(s) and s.both_sides(self)

    def both_sides(self, s):
        ccw1 = ccw(self.p0[0], self.p0[1], s.p0[0], s.p0[1], self.p1[0], self.p1[1])
        ccw2 = ccw(self.p0[0], self.p0[1], s.p1[0], s.p1[1], self.p1[0], self.p1[1])
        if ccw1 == 0 and ccw2 == 0:
            return self.opposite(s.p0) or self.opposite(s.p1)
        else:
            return ccw1 * ccw2 <= 0

    def opposite(self, q):
        return dot(self.p0[0] - q[0], self.p0[1] - q[1], self.p1[0] - q[0], self.p1[1] - q[1]) <= 0

    def internal(self, q):
        return ccw(self.p0[0], self.p0[1], self.p1[0], self.p1[1], q[0], q[1]) == 0 and self.opposite(q)


