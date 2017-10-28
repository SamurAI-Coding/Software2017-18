#include <cstdio>
#include <memory>
#include <boost/process.hpp>

struct RaceCourse;

enum struct Status {
  VALID = 0,
  TIMEOUT = -1,
  INVALID = -2,
  DIED = -3
};

struct Player {
  std::unique_ptr<boost::process::opstream> toAI;
  std::unique_ptr<boost::process::ipstream> fromAI;
  std::unique_ptr<boost::process::child> child;
  string name;
  FILE *logOutput;
  Point position;
  IntVec velocity;
  int timeLeft;
  Status status;
  Player(string command, const RaceCourse &course,
   int xpos, string name, FILE *logFile);
  IntVec play(int c, Player &op, RaceCourse &course);
  void terminate();
};

