#include "raceState.hpp"

bool LineSegment::goesThru(const Point &p) const {
  int minx = min(p1.x, p2.x);
  if (p.x < minx) return false;
  int maxx = max(p1.x, p2.x);
  if (p.x > maxx) return false;
  int miny = min(p1.y, p2.y);
  if (p.y < miny) return false;
  int maxy = max(p1.y, p2.y);
  if (p.y > maxy) return false;
  return (p.y - miny)*(maxx - minx) == (p.x - minx)*(maxy-miny);
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

bool Course::obstacled(Point &from, Point &to) const {
  LineSegment m(from, to);
  int
    x1 = from.x, y1 = from.y,
    x2 = to.x, y2 = to.y;
  if (x2 < 0 || width <= x2 || y2 < 0) return true;
  if (obstacle[x2][y2] == OBSTACLE) return true;
  int xstep, xstart, xend;
  if (x1 < x2) {
    xstep = 1; xstart = max(0, x1); xend = min(width, x2);
  } else {
    xstep = -1; xstart = min(width, x1); xend = max(0, x2);
  }
  if (y1 == y2) {
    for (int x = xstart; x != xend; x += xstep) {
      if (obstacle[x][y1] == OBSTACLE) return true;
    }
    return false;
  }
  int ystep, ystart, yend;
  if (y1 < y2) {
    ystep = 1; ystart = max(0, y1); yend = min(length, y2);
  } else {
    ystep = -1; ystart = min(length, y1); yend = max(0, y2);
  }
  if (x1 == x2) {
    for (int y = ystart; y != yend; y += ystep) {
      if (obstacle[x1][y] == OBSTACLE) return true;
    }
    return false;
  }
  for (int x = xstart; x != xend; x += xstep) {
    int nx = x + xstep;
    for (int y = ystart; y != yend; y += ystep) {
      int ny = y + ystep;
      if ((obstacle[x][y] == OBSTACLE && obstacle[nx][ny] == OBSTACLE &&
	   LineSegment(Point(x, y), Point(nx, ny)).intersects(m)) ||
	  (obstacle[x][ny] == OBSTACLE && obstacle[nx][ny] == OBSTACLE &&
	   LineSegment(Point(x, ny), Point(nx, ny)).intersects(m)) ||
	  (obstacle[nx][y] == OBSTACLE && obstacle[nx][ny] == OBSTACLE &&
	   LineSegment(Point(nx, y), Point(nx, ny)).intersects(m)) ||
	  (obstacle[x][ny] == OBSTACLE && obstacle[nx][y] == OBSTACLE &&
	   LineSegment(Point(x, ny), Point(nx, y)).intersects(m))) {
	return true;
      }
    }
  }
  return false;
}

Course::Course(istream &in) {
  in >> thinkTime >> stepLimit>> width >> length >> vision;
  obstacle = vector<vector<ObstState>>(width);
  for (int x = 0; x != width; x++) {
    obstacle[x] = vector<ObstState>(length);
    for (int y = 0; y != length; y++) {
      obstacle[x][y] = UNKNOWN;
    }
  }
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
    for (int x = 0; x != course.width; x++) {
      int o; in >> o;
      if (y >= 0 && y < course.length) {
	course.obstacle[x][y] = (o == 0 ? NONE : OBSTACLE);
      }
    }
  }
}
