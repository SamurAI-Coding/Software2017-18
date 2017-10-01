#include "raceState.hpp"

int main(int argc, char *argv[]) {
  if (argc < 6 || 8 < argc ) {
    cerr << "Usage: " << argv[0]
	 << " <course file> <player0> <name0> <player1> <name1> "
	 << "[<logFile0> [<logFile1>]]"
	 << endl;
    return 1;
  }
  ifstream in(argv[1]);
  RaceCourse course(in);
  string player0 = argv[2];
  string name0 = argv[3];
  string player1 = argv[4];
  string name1 = argv[5];
  FILE* logFile0 = argc > 6 ? fopen(argv[6], "w") : nullptr;
  FILE* logFile1 = argc > 7 ? fopen(argv[7], "w") : nullptr;
  RaceState state(course, player0, name0, logFile0, player1, name1, logFile1);
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
