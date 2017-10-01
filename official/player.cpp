#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <cctype>

#include "course.hpp"
#include "player.hpp"

static int readInt(FILE *in) {
  char c;
  bool negate = false;
  do {
    c = fgetc(in);
  } while (isblank(c));
  if (c == '-') {
    negate = !negate;
    c = fgetc(in);
  }
  if (!isdigit(c)) {
    cerr << "Illegal response from AI: '" << c << "'" << endl;
    exit(1);
  }
  int v = 0;
  do {
    v = 10*v + c - '0';
    c = fgetc(in);
  } while (isdigit(c));
  return negate ? -v : v;
}

void sendToAI(FILE* toAI, FILE* logOutput, const char *fmt, int value) {
  fprintf(toAI, fmt, value);
  if (logOutput != nullptr) fprintf(logOutput, fmt, value);
}

void flushToAI(FILE* toAI, FILE* logOutput) {
  fflush(toAI);
  if (logOutput != nullptr) fflush(logOutput);
}


Player::Player(string command, const RaceCourse &course, int xpos,
	       string name, FILE* logFile):
  name(name), logOutput(logFile), position(Point(xpos, 0)), velocity(0, 0),
  timeLeft(course.thinkTime) {
  int infoPipe[2];
  int respPipe[2];
  pipe(infoPipe);
  pipe(respPipe);
  processId = fork();
  if (processId == -1) {
    cerr << "Failed to fork an AI process: " << command << endl;
    exit(1);
  } else if (processId == 0) {
    // In child process
    dup2(infoPipe[0], 0);
    close(infoPipe[1]);
    dup2(respPipe[1], 1);
    close(respPipe[0]);
    char *cmd = strcpy(new char[command.size()+1], command.c_str());
    char *const argv[] = { cmd, nullptr };
    char *const envp[] = { nullptr };
    execve(cmd, argv, envp);
  } else {
    toAI = fdopen(infoPipe[1], "w");
    close(infoPipe[0]);
    fromAI = fdopen(respPipe[0], "r");
    close(respPipe[1]);
    sendToAI(toAI, logOutput, "%d\n", course.thinkTime);
    sendToAI(toAI, logOutput, "%d\n", course.stepLimit);
    sendToAI(toAI, logOutput, "%d ", course.width);
    sendToAI(toAI, logOutput, "%d\n", course.length);
    sendToAI(toAI, logOutput, "%d\n", course.vision);
    flushToAI(toAI, logOutput);
    int ans;
    ans = readInt(fromAI);
    if (ans != 0) {
      cerr << "Response at initialization of player <" << command << "> ("
	   << ans << ") is non-zero" << endl;
      exit(1);
    }
  }
}

IntVec Player::play(int c, Player &op, RaceCourse &course) {
  sendToAI(toAI, logOutput, "%d\n", c);
  sendToAI(toAI, logOutput, "%d\n", timeLeft);
  sendToAI(toAI, logOutput, "%d ", position.x);
  sendToAI(toAI, logOutput, "%d ", position.y);
  sendToAI(toAI, logOutput, "%d ", velocity.x);
  sendToAI(toAI, logOutput, "%d\n", velocity.y);
  sendToAI(toAI, logOutput, "%d ", op.position.x);
  sendToAI(toAI, logOutput, "%d ", op.position.y);
  sendToAI(toAI, logOutput, "%d ", op.velocity.x);
  sendToAI(toAI, logOutput, "%d\n", op.velocity.y);
  for (int y = position.y-course.vision;
       y <= position.y+course.vision;
       y++) {
    for (int x = 0; x != course.width; x++) {
      sendToAI(toAI, logOutput, "%d ",
	       (y < 0 || y > course.length ||
		course.obstacle[x][y] ? 1 : 0));
    }
    sendToAI(toAI, logOutput, "\n", 0);
  }
  flushToAI(toAI, logOutput);
  int ax ,ay;
  ax = readInt(fromAI);
  ay = readInt(fromAI);
  return IntVec(ax, ay);
}
    
void Player::terminate() {
  kill(processId, SIGKILL);
}
