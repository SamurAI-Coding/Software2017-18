#include <algorithm>
#include <stdexcept>
#include "course.hpp"

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

bool RaceCourse::collides(const LineSegment &m) const {
  int
    x1 = m.p1.x, y1 = m.p1.y,
    x2 = m.p2.x, y2 = m.p2.y;
  if (obstacle[y2][x2]) return true;
  int xstep = x2 > x1 ? 1 : -1;
  if (y1 == y2) {
    for (int x = x1; x != x2; x += xstep) {
      if (obstacle[y1][x]) return true;
    }
    return false;
  }
  int ystep = y2 > y1 ? 1 : -1;
  if (x1 == x2) {
    for (int y = y1; y != y2; y += ystep) {
      if (obstacle[y][x1]) return true;
    }
    return false;
  }
  for (int y = y1; y != y2; y += ystep) {
    int ny = y + ystep;
    for (int x = x1; x != x2; x += xstep) {
      int nx = x + xstep;
      if (obstacle[y][x] && m.goesThru({x, y})) {
	return true;
      }
      if ((obstacle[y][x] && obstacle[ny][nx] &&
	   LineSegment(Point(x, y), Point(nx, ny)).intersects(m)) ||
	  (obstacle[ny][x] && obstacle[ny][nx] &&
	   LineSegment(Point(x, ny), Point(nx, ny)).intersects(m)) ||
	  (obstacle[y][nx] && obstacle[ny][nx] &&
	   LineSegment(Point(nx, y), Point(nx, ny)).intersects(m)) ||
	  (obstacle[ny][x] && obstacle[y][nx] &&
	   LineSegment(Point(x, ny), Point(nx, y)).intersects(m))) {
	return true;
      }
    }
  }
  return false;
}

ObstacleCol::ObstacleCol(bool obs, size_t size) : cols((int)size) {
  for (int i = 0; i < cols; ++i) {
    col.push_back(obs);
  }
}
ObstacleCol::ObstacleCol(const boost::property_tree::ptree& tree) {
  cols = 0;
  col = {};
  for (const auto& v : tree) {
    col.push_back(v.second.get_value<int>() == 1);
    ++cols;
  }
}
bool ObstacleCol::operator[](int pos) const {
  if (0 <= pos && pos < cols) {
    return col[pos];
  }
  return true;
}
ObstacleCol::const_iterator ObstacleCol::begin() const {
  return col.begin();
}
ObstacleCol::const_iterator ObstacleCol::end() const {
  return col.end();
}

Obstacle::Obstacle_Impl::Obstacle_Impl(const boost::property_tree::ptree& tree, const int length, const int vision)
    : UNDER(true, tree.size() ? tree.front().second.size() : 0),
      OVER(false, tree.size() ? tree.front().second.size() : 0) {
  if (tree.size() == 0) {
    throw std::invalid_argument("raw size is zero");
  }
  const size_t cols = tree.front().second.size();
  rows = 0;
  raw = {};
  for (const auto& v : tree) {
    if (rows >= length + vision) {
      std::cerr << "\e[1m\e[91mWarning\e[0m: \e[4mobstacles row size:" << tree.size() << " is over length:" << length << " + vision:" << vision << ", so the excess was ignored.\e[0m" << std::endl;
      break;
    }
    if (v.second.size() != cols) {
      throw std::invalid_argument("not all col size is equal");
    }
    raw.push_back(ObstacleCol(v.second));
    ++rows;
  }
}
const ObstacleCol& Obstacle::Obstacle_Impl::operator[](int pos) const {
  if (pos < 0) {
    return UNDER;
  }
  if (rows <= pos) {
    return OVER;
  }
  return raw[pos];
}
Obstacle::Obstacle_Impl::const_iterator Obstacle::Obstacle_Impl::begin() const {
  return raw.begin();
}
Obstacle::Obstacle_Impl::const_iterator Obstacle::Obstacle_Impl::end() const {
  return raw.end();
}

Obstacle::Obstacle(const boost::property_tree::ptree& tree, const int length, const int vision) {
  obstacle_ptr = std::make_shared<Obstacle_Impl>(tree, length, vision);
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
  obstacle = Obstacle(courseTree.get_child("obstacles"), length, vision);
}

void IntVec::writeJson(ostream &out) {
  out << "{ \"x\": " << x << ", \"y\": " << y << " }";
}

void RaceCourse::writeJson(ostream &out) {
  out << "{" << endl
      << "   \"filetype\": \"race course\"," << endl
      << "   \"width\": " << width  << ", \"length\": " << length << ',' << endl
      << "   \"vision\": " << vision << ", \"thinkTime\" : " << thinkTime << ", \"stepLimit\" : " << stepLimit << ',' << endl
      << "   \"x0\": " << startX[0] << ", \"x1\": " << startX[1] << ',' << endl;
  out << "  \"obstacles\": [";
  int i = 0;
  for (const auto& col : obstacle) {
    if (i++) {
      out << ",";
      out << endl;
    }
    out << "     [";
    int j = 0;
    for (const auto& obs : col) {
      if (j++) {
        out << ", ";
      }
      out << (obs ? 1 : 0);
    }
    out << "]";
  }
  out << "]" << endl
      << "}" << endl;
}
