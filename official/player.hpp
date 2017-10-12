#include <cstdio>
#include <boost/optional.hpp>
#include <boost/process.hpp>

struct RaceCourse;

class Player {
private:
  boost::optional<boost::process::opstream&> toAI;
  boost::optional<boost::process::ipstream&> fromAI;
  boost::optional<boost::process::child&> child;
public:
  string name;
  FILE *logOutput;
  Point position;
  IntVec velocity;
  int timeLeft;
  Player(string command, const RaceCourse &course,
   int xpos, string name, FILE *logFile);
  IntVec play(int c, Player &op, RaceCourse &course);
  void terminate();
};
