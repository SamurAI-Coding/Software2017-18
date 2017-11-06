#include "raceState.hpp"

#include <boost/program_options.hpp>

int main(int argc, char *argv[]) {
  boost::program_options::options_description desc("<option>");
  desc.add_options()
    ("stdinLogFile0", boost::program_options::value<std::string>()->value_name("<filename>"), "logfile name for player0 standard input")
    ("stdinLogFile1", boost::program_options::value<std::string>()->value_name("<filename>"), "logfile name for player1 standard input")
    ("stderrLogFile0", boost::program_options::value<std::string>()->value_name("<filename>"), "logfile name for player0 standard error output")
    ("stderrLogFile1", boost::program_options::value<std::string>()->value_name("<filename>"), "logfile name for player1 standard error output")
    ("pauseP0", boost::program_options::value<std::string>()->value_name("<command>"), "pause command for player0")
    ("resumeP0", boost::program_options::value<std::string>()->value_name("<command>"), "resume command for player0")
    ("pauseP1", boost::program_options::value<std::string>()->value_name("<command>"), "pause command for player1")
    ("resumeP1", boost::program_options::value<std::string>()->value_name("<command>"), "resume command for player1")
    ("help,h", "produce help");
  boost::program_options::variables_map vm;
  std::vector<std::string> unnamed_args;
  auto help_message = [&] {
    cerr << "Usage: " << argv[0] << " <course file> <player0> <name0> <player1> <name1> [<option>...]" << endl;
    cerr << endl;
    cerr << desc << endl;
  };
  try {
    const auto parsing_result = boost::program_options::parse_command_line(argc, argv, desc);
    boost::program_options::store(parsing_result, vm);
    boost::program_options::notify(vm);
    unnamed_args = boost::program_options::collect_unrecognized(parsing_result.options, boost::program_options::include_positional);
  } catch (std::exception) {
    cerr << "invalid command line arguments" << endl;;
    help_message();
    return 1;
  }
  if (vm.count("help") || unnamed_args.size() != 5) {
    help_message();
    return 1;
  }
  ifstream in(unnamed_args[0]);
  RaceCourse course(in);
  string player0 = unnamed_args[1];
  string name0 = unnamed_args[2];
  string player1 = unnamed_args[3];
  string name1 = unnamed_args[4];
  std::array<Option, 2> options;
  auto take_ofstream = [](const boost::program_options::variables_map& vm, const std::string& target) -> std::shared_ptr<std::ofstream> {
    if (vm.count(target)) {
      return std::shared_ptr<std::ofstream>(new std::ofstream(vm[target].as<std::string>()));
    } else {
      return nullptr;
    }
  };
  auto take_command = [](const boost::program_options::variables_map& vm, const std::string& target) -> boost::optional<std::string> {
    if (vm.count(target)) {
      return vm[target].as<std::string>();
    } else {
      return boost::none;
    }
  };
  for (int i = 0; i < 2; ++i) {
    options[i].stdinLogStream = take_ofstream(vm, "stdinLogFile" + std::to_string(i));
    options[i].stderrLogStream = take_ofstream(vm, "stderrLogFile" + std::to_string(i));
    options[i].pauseCommand = take_command(vm, "pauseP" + std::to_string(i));
    options[i].resumeCommand = take_command(vm, "resumeP" + std::to_string(i));
  }
  RaceState state(course, player0, name0, player1, name1, options);
  for (int c = 0; c != course.stepLimit && !state.playOneStep(c); c++)
    ;
  for (int p = 0; p != 2; p++) {
    state.players[p].terminate();
  }
  cout << "{ \"filetype\": \"race log\"," << endl
       << "\"course\":";
  course.writeJson(cout);
  cout << "," << endl
       << "\"name0\": \"" << name0 << "\", \"name1\": \"" << name1 << "\"," << endl
       << "\"time0\": " << state.goalTime[0]
       << ", \"time1\": " << state.goalTime[1] << ',' << endl;
  cout << "\"log0\": [";
  for (auto i = state.records[0].begin(); i != state.records[0].end(); i++) {
    if (i != state.records[0].begin()) cout << ',';
    cout << endl;
    i->writeJson(cout);
  }
  cout << "]," << endl
       << "\"log1\": [";
  for (auto i = state.records[1].begin(); i != state.records[1].end(); i++) {
    if (i != state.records[1].begin()) cout << ',';
    cout << endl;
    i->writeJson(cout);
  }
  cout << "]" << endl
       << "}" << endl;
  return 0;
}
