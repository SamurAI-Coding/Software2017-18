#include <cstdio>

struct RaceCourse;

struct Player {
  pid_t processId;
  FILE *toAI;
  FILE *fromAI;
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
