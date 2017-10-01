SamurAI Jockey Game Software

                                                            2017/09/30

This directory contains a draft version of software for the SamurAI
Jockey game, to be used in the SamurAI Coding 2017-2018 contest.

1. Directory Structure

The following subdirectories are in the top level directory.
  documents:
    Documents on the game rules and data file structures.
  official:
    Game execution management system software.
  viewer:
    Visualizing software for game logs.
  editor:
    Editing software for race courses.
  player:
    Sample AI players.
  samples:
    Samples of race course and race logs.

2. Watching a Sample Race

Open viewer/viewer.html in a web browser.
Clicking the button labeled "Load Log" pops up a file selection dialog.
Here, choose "samples/sample.racelog" in the distribution.
Clicking the "Start" button shows the race.
For more details, click the "Help" button to browse the manual page.

3. Documents

The "documents" directory contains the following documents.

  rule-en.pdf: Game rule draft
  rule-jp.pdf: Game rule draft (in Japanese)
  datafiles.pdf: Desciption of data file formats

4. Installation

For the execution management system and sample players,
run "make" in the respective directories.
A C++ language system conforming C++11 and the boost library is required.

For the viewer and the editor, no installation is needed.
They can be executed immediately with web browsers capable of executing Javascript.

5. Usage

(1) Execution Management System

Arguments in the command line should be specified as follows.
　　./official <course file> <player0> <name0> <player1> <name1> [ <log0> <log1> ]

  <course file>:
    File describing the course to be used in the race.
    This is usually a file output by the editor by clicking the "Write" button.
    A sample can be found in "sample/sample-course.smrjky".
  <player0>, <player1>:
    Pathnames for executables of AI programs to compete in the race.
    A sample player executable is made as "player/greedy"
    by running "make" in the "player" directory.
  <name0>, <name1>:
    Names of AI programs to be used by the race viewer.
  <log0>, <log1>
    When these pathnames are specified, copies of data sent to AI programs
    will be stored.
    These might be useful for debugging AI programs.

(2) Course Editor

  Open "editor/editor.html" with a web browser.
  A mannual page can be found by clicking the "Help" button at top right.

(3) Race Viewer

  Open "viewer/viewer.html" with a web browser.
  A mannual page can be found by clicking the "Help" button at top right.

6. Known Bugs and Features Not Implemented (yet)

- Restrictions of execution of AI programs
  - Management of think time is not implemented yet.
  - File and network accesses are not disabled.

