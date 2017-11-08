#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::property_tree;

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
  void writeJson(ostream &out);
};

typedef IntVec Point;

class Obstacle {
public:
  class ObstacleCol {
  private:
    const bool DEFAULT;
    int cols;
    std::vector<bool> col;
  public:
    ObstacleCol(bool outer);
    ObstacleCol(const boost::property_tree::ptree& tree);
    bool operator[](int pos) const;
  };
private:
  const ObstacleCol UNDER;
  const ObstacleCol OVER;
  int rows;
  std::vector<std::shared_ptr<ObstacleCol>> raw;
public:
  Obstacle(const boost::property_tree::ptree& tree);
  const Obstacle::ObstacleCol& operator[](int pos);
};

struct LineSegment {
  Point p1, p2;
  LineSegment() {};
  LineSegment(Point p1, Point p2): p1(p1), p2(p2) {};
  bool goesThru(const Point &p) const;
  bool intersects(const LineSegment &l) const;
};

struct RaceCourse {
  ptree courseTree;
  int width, length;
  int vision;
  int thinkTime, stepLimit;
  int startX[2];
  vector <vector <bool>> obstacle;
  RaceCourse(istream &in);
  bool collides(const LineSegment &m) const;
  void writeJson(ostream &out);
};
