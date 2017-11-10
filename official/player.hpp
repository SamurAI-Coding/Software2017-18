#include <cstdio>
#include <memory>
#include <thread>
#include <istream>
#include <ostream>
#include <boost/process.hpp>
#include <boost/optional.hpp>

struct RaceCourse;
struct Option {
  std::shared_ptr<std::ofstream> stdinLogStream;
  std::shared_ptr<std::ofstream> stderrLogStream;
  boost::optional<std::string> pauseCommand;
  boost::optional<std::string> resumeCommand;
};

enum struct Status {
  VALID = 0,
  TIMEOUT = -1,
  INVALID = -2,
  DIED = -3
};

class Logger {
private:
  std::thread thread;
  std::future<void> future;
public:
  std::shared_ptr<std::mutex> mutex;
  Logger(std::unique_ptr<std::istream> input, std::shared_ptr<std::ostream> output, int MAX_SIZE);
  ~Logger();
};

struct Player {
  std::unique_ptr<boost::process::opstream> toAI;
  std::unique_ptr<boost::process::ipstream> fromAI;
  std::unique_ptr<boost::process::child> child;
  string name;
  Point position;
  IntVec velocity;
  int64_t timeLeft;
  Status status;
  std::shared_ptr<std::ofstream> stdinLogStream;
  std::shared_ptr<std::ofstream> stderrLogStream;
  boost::optional<std::string> pauseCommand;
  boost::optional<std::string> resumeCommand;
  std::unique_ptr<Logger> stderrLogger;
  Player(string command, const RaceCourse &course,
   int xpos, string name, const Option &opt);
  IntVec play(int c, Player &op, RaceCourse &course);
  void terminate();
};

