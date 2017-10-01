#include <map>
#include <queue>
#include <limits>
#include "raceState.hpp"

const int searchDepth = 7;

struct PlayerState {
  Point position;
  IntVec velocity;
  bool operator<(const PlayerState &ps) const {
    return
      position != ps.position ?
      position < ps.position :
      velocity < ps.velocity;
  }
  PlayerState(Point p, IntVec v): position(p), velocity(v) {}
};

struct Candidate {
  int step;			// Steps needed to come here
  PlayerState state;		// State of the player
  Candidate *from;		// Came here from this place
  IntVec how;			//   with this acceleration
  Candidate(int t, PlayerState s, Candidate *f, IntVec h):
    step(t), state(s), from(f), how(h) {}
};
  
IntVec play(RaceState &rs, const Course &course) {
  queue <Candidate *> candidates;
  map <PlayerState, Candidate *> reached;
  PlayerState initial(rs.position, rs.velocity);
  Candidate initialCand(0, initial, nullptr, IntVec(0, 0));
  reached[initial] = &initialCand;
  Candidate *best = &initialCand;
  double goalTime = numeric_limits<double>::max();
  candidates.push(&initialCand);
  do {
    Candidate *c = candidates.front();
    candidates.pop();
    for (int cay = 1; cay != -2; cay--) {
      for (int cax = -1; cax != 2; cax++) {
	IntVec nextVelo = c->state.velocity + IntVec(cax, cay);
	Point nextPos = c->state.position + nextVelo;
	if ((c->step != 0 ||
	     !LineSegment(c->state.position, nextPos).goesThru(rs.oppPosition)) &&
	    !course.obstacled(c->state.position, nextPos)) {
	  PlayerState next(nextPos, nextVelo);
	  Candidate *nextCand =
	    new Candidate(c->step+1, next, c, IntVec(cax, cay));
	  if (nextPos.y >= course.length) {
	    double t = c->step +
	      (double)(course.length - c->state.position.y)/nextVelo.y;
	    if (t < goalTime) {
	      best = nextCand;
	      goalTime = t;
	    }
	  } else if (reached.count(next) == 0) {
	    if (c->step < searchDepth && nextPos.y < course.length) {
	      candidates.push(nextCand);
	    }
	    reached[next] = nextCand;
	    if (nextPos.y > best->state.position.y) {
	      best = nextCand;
	    }
	  }
	}
      }
    }
  } while (!candidates.empty());
  if (best == &initialCand) {
    // No good move found
    // Slowing down for a while might be a good strategy
    int ax = 0, ay = 0;
    if (rs.velocity.x < 0) ax += 1;
    else if (rs.velocity.x > 0) ax -= 1;
    if (rs.velocity.y < 0) ay += 1;
    else if (rs.velocity.y > 0) ay -= 1;
    return IntVec(ax, ay);
  }
  Candidate *c = best;
  while (c->from != &initialCand) c = c->from;
  return c->how;
}

int main(int argc, char *argv[]) {
  Course course(cin);
  cout << 0 << endl;
  cout.flush();
  while (true) {
    RaceState rs(cin, course);
    IntVec accel = play(rs, course);
    cout << accel.x << ' ' << accel.y << endl;
  }
}
