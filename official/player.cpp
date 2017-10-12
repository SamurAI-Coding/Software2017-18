#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <cctype>

#include "course.hpp"
#include "player.hpp"

static int readInt(std::unique_ptr<boost::process::ipstream>&  in) {
  int num;
  std::string str;
  *in >> str;
  try {
    num = std::stoi(str);
  } catch (const std::invalid_argument& e) {
    std::cerr << "input invalid argument from AI : " << str << std::endl;
    std::cerr << e.what() << std::endl;
    exit(1);
  } catch (const std::out_of_range& e) {
    std::cerr << "input out of int range value from AI : " << str << std::endl;
    std::cerr << e.what() << std::endl;
    exit(1);
  }
  return num;
}

void sendToAI(std::unique_ptr<boost::process::opstream>&  toAI, FILE* logOutput, const char *fmt, int value) {
  int n = std::snprintf(nullptr, 0, fmt, value);
  auto cstr = new std::unique_ptr<char[]>(new char[n + 2]);
  std::snprintf(cstr->get(), n + 1, fmt, value);
  std::string str(cstr->get());
  *toAI << str;
  if (logOutput != nullptr) fprintf(logOutput, fmt, value);
}

void flushToAI(std::unique_ptr<boost::process::opstream>& toAI, FILE* logOutput) {
  toAI->flush();
  if (logOutput != nullptr) fflush(logOutput);
}


Player::Player(string command, const RaceCourse &course, int xpos,
         string name, FILE* logFile):
  name(name), logOutput(logFile), position(Point(xpos, 0)), velocity(0, 0),
  timeLeft(course.thinkTime) {
  auto env = boost::this_process::environment();
  toAI = std::unique_ptr<boost::process::opstream>(new boost::process::opstream);
  fromAI = std::unique_ptr<boost::process::ipstream>(new boost::process::ipstream);
  child = std::unique_ptr<boost::process::child>(new boost::process::child(
    command,
    boost::process::std_out > *fromAI,
    boost::process::std_err > stderr,
    boost::process::std_in < *toAI,
    env
  ));
  sendToAI(toAI, logOutput, "%d\n", course.thinkTime);
  sendToAI(toAI, logOutput, "%d\n", course.stepLimit);
  sendToAI(toAI, logOutput, "%d ", course.width);
  sendToAI(toAI, logOutput, "%d\n", course.length);
  sendToAI(toAI, logOutput, "%d\n", course.vision);
  flushToAI(toAI, logOutput);
  int ans = readInt(fromAI);
  if (ans != 0) {
    cerr << "Response at initialization of player <" << command << "> ("
   << ans << ") is non-zero" << endl;
    exit(1);
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
  child->terminate();
}
