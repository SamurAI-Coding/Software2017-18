#include "raceState.hpp"

StepRecord::StepRecord(int step, PlayerState bfr, IntVec accel,
			 int res, PlayerState aft):
  step(step), before(bfr), acceleration(accel), result(res), after(aft) {
}


RaceState::RaceState(RaceCourse &course,
		     string &player0, string &name0,
		     string &player1, string &name1,
         const std::array<Option, 2>& options):
  course(&course),
  players{
      Player(player0, course, course.startX[0], name0, options[0]),
	Player(player1, course, course.startX[1], name1, options[1])} {
  goalTime[0] = goalTime[1] = 2*course.stepLimit;
  goaled[0] = goaled[1] = false;
}

bool RaceState::playOneStep(int c) {
  Point nextPosition[2];
  IntVec nextVelocity[2];
  IntVec accel[2];
  int res[2];
  LineSegment move[2];
  for (int p = 0; p != 2; p++) {
    if (players[p].status != Status::VALID) {
      res[p] = FUNNY;
      accel[p] = IntVec(0, 0);
      nextVelocity[p] = IntVec(0, 0);
      nextPosition[p] = players[p].position;
      continue;
    }
    if (!goaled[p]) {
      res[p] = NORMAL;
      accel[p] = players[p].play(c, players[1-p], *course);
      if (players[p].status != Status::VALID) {
        res[p] = FUNNY;
        accel[p] = IntVec(0, 0);
        nextVelocity[p] = IntVec(0, 0);
        nextPosition[p] = players[p].position;
        continue;
      }
      nextVelocity[p] = players[p].velocity + accel[p];
      nextPosition[p] = players[p].position + nextVelocity[p];
      move[p] = LineSegment(players[p].position, nextPosition[p]);
      if (course->collides(move[p])) {
	res[p] = STOPPED;
      }
    }
  }
  if (!goaled[0] && !goaled[1]) {
    // Check collision if both sides have not reached the goal yet
    // Going through the opponent's position is not allowed even with precedence
    if (res[0] != FUNNY && res[1] != FUNNY) {
      for (int i = 0; i < 2; ++i) {
        if (move[i].goesThru(players[1 - i].position)) {
          res[i] = STOPPED;
        }
      }
    }
    // Check intersection
    bool moveCollision = move[0].intersects(move[1]) && res[0] == NORMAL && res[1] == NORMAL;
    if (moveCollision) {
      bool prec0 =
	(players[0].position.y < players[1].position.y) ||
	((players[0].position.y == players[1].position.y) &&
	 players[0].position.x < players[1].position.x);
      if (prec0) {
	// Player 0 has the precedence
	res[1] = STOPPED;
      } else {
	res[0] = STOPPED;
      }
    }
  }
  for (int p = 0; p != 2; p++) {
    if (!records[p].empty() && records[p].back().result == FUNNY) {
      continue;
    }
    if (!goaled[p]) {
      records[p].emplace_back
	(c,
	 PlayerState(players[p].position, players[p].velocity),
	 accel[p],
	 res[p],
	 PlayerState(res[p] == NORMAL ? nextPosition[p] : players[p].position,
		     nextVelocity[p]));
      players[p].velocity = nextVelocity[p];
      if (res[p] == NORMAL) {
	if (nextPosition[p].y >= course->length) {
	  goaled[p] = true;
	  players[p].status = Status::GOALED;
	  goalTime[p] =
	    c-1+
	    (double)(course->length- players[p].position.y)/nextVelocity[p].y;
	}
	players[p].position = nextPosition[p];
      }
    }
  }
  return goaled[0] && goaled[1];
}

void StepRecord::writeJson(ostream &out) {
  out << '{' << endl
      << "  \"step\": " << step << ',' << endl
      << "  \"before\": ";
  before.position.writeJson(out);
  out << ',' << endl
      << "  \"velocity\": ";
  before.velocity.writeJson(out);
  out << ',' << endl
      << "  \"acceleration\": ";
  acceleration.writeJson(out);
  out << ',' << endl
      << "  \"result\": " << result << ',' << endl
      << "  \"after\": ";
  after.position.writeJson(out);
  out << endl
      << '}';
}

