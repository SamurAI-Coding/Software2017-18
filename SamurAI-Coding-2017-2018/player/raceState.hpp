#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <map>

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

enum struct ObstState { UNKNOWN, OBSTACLE, NONE };

class ObstacleCol {
private:
  using const_iterator = std::vector<ObstState>::const_iterator;
  int cols;
  std::vector<ObstState> col;
public:
  ObstacleCol() = default;
  ObstacleCol(ObstState obs, int size);
  ObstacleCol(const std::vector<int>& arr);
  ObstState operator[](int pos) const;
  const_iterator begin() const;
  const_iterator end() const;
};
class Obstacle {
private:
  class Obstacle_Impl {
  private:
    using const_iterator = std::map<int, ObstacleCol>::const_iterator;
    const ObstacleCol UNDER;
    const ObstacleCol UNKNOWN;
    int rows;
    std::map<int, ObstacleCol> raw;
  public:
    Obstacle_Impl(int width);
    const ObstacleCol& operator[](int pos) const;
    const_iterator begin() const;
    const_iterator end() const;
    void put(int y, const std::vector<int>& arr);
  };
  std::shared_ptr<Obstacle_Impl> obstacle_ptr;
public:
  const ObstacleCol& operator[](int pos) const;
  Obstacle() = default;
  Obstacle(int width);
  decltype(obstacle_ptr->begin()) begin() const;
  decltype(obstacle_ptr->end()) end() const;
  void put(int y, const std::vector<int>& arr);
};

struct Course {
  int width, length;
  int vision;
  int thinkTime, stepLimit;
  Obstacle obstacle;
  bool obstacled(const Point &from, const Point &to) const;
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
