#include <list>
#include <cstdio>
#include <memory>
#include <fstream>

#include <boost/optional.hpp>

#include "course.hpp"
#include "player.hpp"

const int FUNNY = -1;
const int NORMAL = 0;
const int STOPPED = 1;

struct PlayerState {
  Point position;
  IntVec velocity;
  PlayerState(Point p, IntVec v): position(p), velocity(v) {}
  PlayerState() {}
  bool operator<(const PlayerState &theOther) {
    if (position.x != theOther.position.x) return position.x < theOther.position.x;
    if (position.y != theOther.position.y) return position.y < theOther.position.y;
    if (velocity.x != theOther.velocity.x) return velocity.x < theOther.velocity.x;
    if (velocity.y != theOther.velocity.y) return velocity.y < theOther.velocity.y;
    return false;
  }
};

struct StepRecord {
  int step;
  PlayerState before;
  IntVec acceleration;
  int result;
  PlayerState after;
  StepRecord(int step, PlayerState before, IntVec accel, int res, PlayerState after);
  void writeJson(ostream &out);
};

ostream &operator<<(ostream &out, const StepRecord &r);

struct Option;

struct RaceState {
  RaceCourse *course;
  Player players[2];
  bool goaled[2];
  double goalTime[2];
  list<StepRecord> records[2];
  RaceState(RaceCourse &course,
	    string& player0, string &name0,
	    string &player1, string &name1, const std::array<Option, 2>& options);
  bool playOneStep(int c);
};

