#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct IntVec {
  int x, y;
  IntVec(int x = 0, int y = 0): x(x), y(y) {};
  inline IntVec operator+(const IntVec &another) const {
    return IntVec(x+another.x, y+another.y);
  }
  inline bool operator==(const IntVec &another) const {
    return x == another.x && y == another.y;
  }
  inline bool operator!=(const IntVec &another) const {
    return !(*this == another);
  }
  inline bool operator<(const IntVec &another) const {
    return y != another.y ? y < another.y : x < another.x;
  }
};

typedef IntVec Point;

struct LineSegment {
  Point p1, p2;
  LineSegment() {};
  LineSegment(Point p1, Point p2): p1(p1), p2(p2) {};
  bool goesThru(const Point &p) const;
  bool intersects(const LineSegment &l) const;
};

enum ObstState { UNKNOWN, OBSTACLE, NONE };

struct Course {
  int width, length;
  int vision;
  int thinkTime, stepLimit;
  vector <vector <ObstState>> obstacle;
  bool obstacled(Point &from, Point &to) const;
  Course(istream &in);
};

struct RaceState {
  int step;
  int timeLeft;
  Point position, oppPosition;
  IntVec velocity, oppVelocity;
  RaceState(istream &in, Course &course);
  bool collides(const LineSegment &m) const;
};
