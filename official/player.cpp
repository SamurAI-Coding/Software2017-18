#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <cctype>
#include <future>
#include <thread>
#include <chrono>
#include <boost/optional.hpp>

#include "course.hpp"
#include "player.hpp"

static boost::optional<int> readInt(std::unique_ptr<boost::process::ipstream>& in) {
  if (!in) {
    return boost::none;
  }
  int num;
  std::string str;
  *in >> str;
  try {
    num = std::stoi(str);
  } catch (const std::invalid_argument& e) {
    std::cerr << "input invalid argument from AI : \"" << str << "\""<< std::endl;
    std::cerr << e.what() << std::endl;
    return boost::none;
  } catch (const std::out_of_range& e) {
    std::cerr << "input out of int range value from AI : \"" << str << "\"" << std::endl;
    std::cerr << e.what() << std::endl;
    return boost::none;
  }
  return num;
}

static void handShake(std::unique_ptr<boost::process::ipstream> in, std::promise<std::pair<std::unique_ptr<boost::process::ipstream>, boost::optional<int>>> p)
{
  auto ans = readInt(in);
  p.set_value(make_pair(std::move(in), ans));
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
  timeLeft(course.thinkTime), status(Status::VALID) {
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
  std::promise<std::pair<std::unique_ptr<boost::process::ipstream>, boost::optional<int>>> promise;
  std::future<std::pair<std::unique_ptr<boost::process::ipstream>, boost::optional<int>>> future = promise.get_future();
  std::chrono::milliseconds remain(timeLeft);
  std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
  std::thread thread(handShake, std::move(fromAI), std::move(promise));
  std::future_status result = future.wait_for(remain);
  std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
  timeLeft -= std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  thread.join();
  if (result == std::future_status::timeout) {
    status = Status::TIMEOUT;
    std::cerr << "player : " << name << " has been timeouted" << std::endl;
    return;
  }
  auto ret = future.get();
  fromAI = std::move(ret.first);
  auto ans = ret.second;
  if (!ans || ans.get() != 0) {
    cerr << "Response at initialization of player <" << command << "> ("
   << ans << ") is non-zero" << endl;
    status = Status::INVALID;
  }
}

static void readAct(std::unique_ptr<boost::process::ipstream> in, std::promise<std::pair<std::unique_ptr<boost::process::ipstream>, boost::optional<std::pair<int, int>>>> p)
{
  auto ax = readInt(in);
  auto ay = readInt(in);

  if (ax && ay) {
    p.set_value(make_pair(std::move(in), make_pair(ax.get(), ay.get())));
  } else {
    p.set_value(make_pair(std::move(in), boost::none));
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
  std::promise<std::pair<std::unique_ptr<boost::process::ipstream>, boost::optional<std::pair<int, int>>>> promise;
  std::future<std::pair<std::unique_ptr<boost::process::ipstream>, boost::optional<std::pair<int, int>>>> future = promise.get_future();
  std::chrono::milliseconds remain(timeLeft);
  std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
  std::thread thread(readAct, std::move(fromAI), std::move(promise));
  std::future_status result = future.wait_for(remain);
  std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
  timeLeft -= std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  thread.join();
  if (result == std::future_status::timeout) {
    status = Status::TIMEOUT;
    std::cerr << "player : " << name << " has been timeouted" << std::endl;
    return IntVec();
  }
  auto ret = future.get();
  fromAI = std::move(ret.first);
  if (ret.second) {
    auto val = ret.second.get();
    if (val.first < -1 || 1 < val.first
      || val.second < -1 || 1 < val.second) {
      std::cerr << "acceleration value must be from -1 to 1 each axis, but player : " << name << " saied : (" << val.first << ", " << val.second << ")" << std::endl;
      status = Status::INVALID;
      return IntVec();
    }
    return IntVec(val.first, val.second);
  } else {
    if (!child->running()) {
      std::cerr << "player : \"" << name << "\" is died." << std::endl;
      std::cerr << "\texit code : " << child->exit_code() << std::endl;
      status = Status::DIED;
      return IntVec();
    }
    status = Status::INVALID;
    return IntVec();
  }
}

void Player::terminate() {
  child->terminate();
}

