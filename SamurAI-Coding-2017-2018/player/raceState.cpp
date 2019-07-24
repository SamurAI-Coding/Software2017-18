#include "raceState.hpp"

static int dot(int x1, int y1, int x2, int y2) {
  return x1 * x2 + y1 * y2;
}

static int cross(int x1, int y1, int x2, int y2) {
  return x1 * y2 - x2 * y1;
}

static int ccw(int x1, int y1, int x2, int y2, int x3, int y3) {
  return cross(x2 - x1, y2 - y1, x3 - x2, y3 - y2);
}

bool LineSegment::goesThru(const Point &p) const {
  int minx = min(p1.x, p2.x);
  if (p.x < minx) return false;
  int maxx = max(p1.x, p2.x);
  if (p.x > maxx) return false;
  int miny = min(p1.y, p2.y);
  if (p.y < miny) return false;
  int maxy = max(p1.y, p2.y);
  if (p.y > maxy) return false;
  return ccw(p1.x, p1.y, p2.x, p2.y, p.x, p.y) == 0 && dot(p1.x - p.x, p1.y - p.y, p2.x - p.x, p2.y - p.y) <= 0;
}

bool LineSegment::intersects(const LineSegment &l) const {
  int minx = min(p1.x, p2.x);
  int maxx = max(p1.x, p2.x);
  int minlx = min(l.p1.x, l.p2.x);
  int maxlx = max(l.p1.x, l.p2.x);
  if (maxx < minlx || maxlx < minx ) return false;
  int miny = min(p1.y, p2.y);
  int maxy = max(p1.y, p2.y);
  int minly = min(l.p1.y, l.p2.y);
  int maxly = max(l.p1.y, l.p2.y);
  if (maxy < minly || maxly < miny ) return false;
  int d1 = (p1.x-l.p1.x)*(l.p2.y-l.p1.y)-(p1.y-l.p1.y)*(l.p2.x-l.p1.x);
  int d2 = (p2.x-l.p1.x)*(l.p2.y-l.p1.y)-(p2.y-l.p1.y)*(l.p2.x-l.p1.x);
  if (d1*d2 > 0) return false; 
  int d3 = (l.p1.x-p1.x)*(p2.y-p1.y)-(l.p1.y-p1.y)*(p2.x-p1.x);
  int d4 = (l.p2.x-p1.x)*(p2.y-p1.y)-(l.p2.y-p1.y)*(p2.x-p1.x);
  if (d3*d4 > 0) return false; 
  return true;
}

bool Course::obstacled(const Point &from, const Point &to) const {
  LineSegment m(from, to);
  int
    x1 = from.x, y1 = from.y,
    x2 = to.x, y2 = to.y;
  if (obstacle[y2][x2] == ObstState::OBSTACLE) return true;
  int xstep = x2 > x1 ? 1 : -1;
  if (y1 == y2) {
    for (int x = x1; x != x2; x += xstep) {
      if (obstacle[y1][x] == ObstState::OBSTACLE) return true;
    }
    return false;
  }
  int ystep = y2 > y1 ? 1 : -1;
  if (x1 == x2) {
    for (int y = y1; y != y2; y += ystep) {
      if (obstacle[y][x1] == ObstState::OBSTACLE) return true;
    }
    return false;
  }
  for (int y = y1; y != y2; y += ystep) {
    int ny = y + ystep;
    for (int x = x1; x != x2; x += xstep) {
      int nx = x + xstep;
      if (obstacle[y][x] == ObstState::OBSTACLE && m.goesThru({x, y})) {
	return true;
      }
      if ((obstacle[y][x] == ObstState::OBSTACLE && obstacle[ny][nx] == ObstState::OBSTACLE &&
     LineSegment(Point(x, y), Point(nx, ny)).intersects(m)) ||
    (obstacle[ny][x] == ObstState::OBSTACLE && obstacle[ny][nx] == ObstState::OBSTACLE &&
     LineSegment(Point(x, ny), Point(nx, ny)).intersects(m)) ||
    (obstacle[y][nx] == ObstState::OBSTACLE && obstacle[ny][nx] == ObstState::OBSTACLE &&
     LineSegment(Point(nx, y), Point(nx, ny)).intersects(m)) ||
    (obstacle[ny][x] == ObstState::OBSTACLE && obstacle[y][nx] == ObstState::OBSTACLE &&
     LineSegment(Point(x, ny), Point(nx, y)).intersects(m))) {
  return true;
      }
    }
  }
  return false;
}

ObstacleCol::ObstacleCol(ObstState obs, int size) : cols(size) {
  for (int i = 0; i < cols; ++i) {
    col.push_back(obs);
  }
}
ObstacleCol::ObstacleCol(const std::vector<int>& arr) {
  cols = 0;
  col = {};
  for (const auto& v : arr) {
    col.push_back(v == 1 ? ObstState::OBSTACLE : ObstState::NONE);
    ++cols;
  }
}
ObstState ObstacleCol::operator[](int pos) const {
  if (0 <= pos && pos < cols) {
    return col[pos];
  }
  return ObstState::OBSTACLE;
}
ObstacleCol::const_iterator ObstacleCol::begin() const {
  return col.begin();
}
ObstacleCol::const_iterator ObstacleCol::end() const {
  return col.end();
}

Obstacle::Obstacle_Impl::Obstacle_Impl(int width) : UNDER(ObstState::OBSTACLE, width), UNKNOWN(ObstState::UNKNOWN, width), rows(0), raw(){}
const ObstacleCol& Obstacle::Obstacle_Impl::operator[](int pos) const {
  if (pos < 0) {
    return UNDER;
  }
  auto ite = raw.find(pos);
  if (ite != raw.end()) {
    return ite->second;
  } else {
    return UNKNOWN;
  }
}
Obstacle::Obstacle_Impl::const_iterator Obstacle::Obstacle_Impl::begin() const {
  return raw.begin();
}
Obstacle::Obstacle_Impl::const_iterator Obstacle::Obstacle_Impl::end() const {
  return raw.end();
}
void Obstacle::Obstacle_Impl::put(int y, const std::vector<int>& arr) {
  if (y < 0) {
    return;
  }
  if (raw.count(y)) {
    return;
  }
  raw[y] = ObstacleCol(arr);
}

Obstacle::Obstacle(int width) {
  obstacle_ptr = std::make_shared<Obstacle_Impl>(width);
}
const ObstacleCol& Obstacle::operator[](int pos) const {
  return (*obstacle_ptr)[pos];
}
decltype(Obstacle::obstacle_ptr->begin()) Obstacle::begin() const {
  return obstacle_ptr->begin();
}
decltype(Obstacle::obstacle_ptr->end()) Obstacle::end() const {
  return obstacle_ptr->end();
}
void Obstacle::put(int y, const std::vector<int>& arr) {
  obstacle_ptr->put(y, arr);
}

Course::Course(istream &in) {
  in >> thinkTime >> stepLimit>> width >> length >> vision;
  obstacle = Obstacle(width);
}

RaceState::RaceState(istream &in, Course &course) {
  in >> step
     >> timeLeft
     >> position.x >> position.y
     >> velocity.x >> velocity.y
     >> oppPosition.x >> oppPosition.y
     >> oppVelocity.x >> oppVelocity.y;
  for (int y = position.y - course.vision;
       y <= position.y + course.vision;
       y++) {
    std::vector<int> arr;
    for (int x = 0; x != course.width; x++) {
      int o; in >> o;
      arr.push_back(o);
    }
    course.obstacle.put(y, arr);
  }
}
