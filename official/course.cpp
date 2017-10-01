#include <algorithm>
#include "course.hpp"

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

bool Obstacle::intersects(const LineSegment &move) const {
  int minx = min(move.p1.x, move.p2.x);
  int maxx = max(move.p1.x, move.p2.x);
  if (maxx < x-0.5 || x+w-0.5 < minx) return false;
  int miny = min(move.p1.y, move.p2.y);
  int maxy = max(move.p1.y, move.p2.y);
  if (maxy < y || y < miny) return false;
  LineSegment move2(Point(2*move.p1.x, 2*move.p1.y), Point(2*move.p2.x, 2*move.p2.y));
  Point nw(2*x-1, 2*y+1);
  Point ne(2*(x+w)-1, 2*y+1);
  Point sw(2*x-1, 2*y-1);
  Point se(2*(x+w)-1, 2*y-1);
  return
    LineSegment(nw, sw).intersects(move2) ||
    LineSegment(ne, se).intersects(move2) ||
    LineSegment(nw, ne).intersects(move2) ||
    LineSegment(sw, se).intersects(move2);
}

bool RaceCourse::collides(const LineSegment &m) const {
  int
    x1 = m.p1.x, y1 = m.p1.y,
    x2 = m.p2.x, y2 = m.p2.y;
  if (obstacle[x2][y2]) return true;
  int xstep = x2 > x1 ? 1 : -1;
  if (y1 == y2) {
    for (int x = x1; x != x2; x += xstep) {
      if (obstacle[x][y1]) return true;
    }
    return false;
  }
  int ystep = y2 > y1 ? 1 : -1;
  if (x1 == x2) {
    for (int y = y1; y != y2; y += ystep) {
      if (obstacle[x1][y]) return true;
    }
    return false;
  }
  for (int x = x1; x != x2; x += xstep) {
    int nx = x + xstep;
    for (int y = y1; y != y2; y += ystep) {
      int ny = y + ystep;
      if ((obstacle[x][y] && obstacle[nx][ny] &&
	   LineSegment(Point(x, y), Point(nx, ny)).intersects(m)) ||
	  (obstacle[x][ny] && obstacle[nx][ny] &&
	   LineSegment(Point(x, ny), Point(nx, ny)).intersects(m)) ||
	  (obstacle[nx][y] && obstacle[nx][ny] &&
	   LineSegment(Point(nx, y), Point(nx, ny)).intersects(m)) ||
	  (obstacle[x][ny] && obstacle[nx][y] &&
	   LineSegment(Point(x, ny), Point(nx, y)).intersects(m))) {
	return true;
      }
    }
  }
  return false;
}

const string CourseDataFileType = "race course";

RaceCourse::RaceCourse(istream &in) {
  read_json(in, courseTree);
  if (!courseTree.get_child_optional("filetype") ||
      courseTree.get<string>("filetype") != CourseDataFileType) {
    cerr << "The input file does not contain race course data" << endl;
    exit(1);
  }
  width = courseTree.get<int>("width");
  length = courseTree.get<int>("length");
  vision = courseTree.get<int>("vision");
  thinkTime = courseTree.get<int>("thinkTime");
  stepLimit = courseTree.get<int>("stepLimit");
  startX[0] = courseTree.get<int>("x0");
  startX[1] = courseTree.get<int>("x1");
  int y = 0;
  obstacle = vector<vector<bool>>(width);
  for (int x = 0; x != width; x++) obstacle[x] = vector<bool>(length);
  for (auto &row: courseTree.get_child("obstacles")) {
    int x = 0;
    for (auto &obst: row.second)
      obstacle[x++][y] = (obst.second.get_value<int>() != 0);
    y++;
  }
}

void IntVec::writeJson(ostream &out) {
  out << "{ \"x\": " << x << ", \"y\": " << y << " }";
}

void Obstacle::writeJson(ostream &out) {
  out << "{ \"x\": " << x << ", \"w\": " << w << " }";
}

void RaceCourse::writeJson(ostream &out) {
  out << "{" << endl
      << "   \"filetype\": \"race course\"," << endl
      << "   \"width\": " << width  << ", \"length\": " << length << ',' << endl
      << "   \"x0\": " << startX[0] << ", \"x1\": " << startX[1] << ',' << endl;
  out << "  \"obstacles\": [";
  for (int y = 0; y != length; y++) {
    out << "     [";
    for (int x = 0; x != width; x++) {
      out << (obstacle[x][y] ? '1' : '0')
	  << (x == width-1 ? "" : ", ");
    }
    out << "]"
	<< (y == length-1 ? "" : ",")
	<< endl;
  }
  out << "]" << endl
      << "}" << endl;
}
