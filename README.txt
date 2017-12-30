SamurAI Jockey Game Software

                                                            2017/12/30

This directory contains software for the SamurAI Jockey game,
to be used in the SamurAI Coding 2017-18 contest.

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

  rule-en.pdf: Game rule
  rule-jp.pdf: Game rule (in Japanese)
  datafiles.pdf: Desciption of data file formats

4. Installation

For the execution management system and sample players,
run "make" in the respective directories.
A C++ language system conforming C++11 and the boost library is required.

The boost library you install must be Version 1.64.0 or higher.
Note the version if you install it using package manager.
Especially, apt : Advanced Package Tool of Ubuntu
cannot install it from their standard package archives.
About an installation, the following is useful.
  http://www.boost.org/doc/libs/1_66_0/more/getting_started/index.html
You have an option of full installation, but our game manager requires
  system, filesystem, program_options
libraries.  So, the minimun configuration is that running b2 program w/ following options
  --with-system --with-filesystem --with-program_options
Please refer the appendix for details.

We administer the following GitHub Release page where our software will be distributed on a trial basis.
  https://github.com/SamurAI-Coding/Software2017-18/releases/latest
The game manager there is static-linked w/ boost libraries,
so you need not install boost if you use it.

For the viewer and the editor, no installation is needed.
They can be executed immediately with web browsers capable of executing Javascript.

5. Usage

(1) Execution Management System

Arguments in the command line should be specified as follows.
　　./official <course file> <player0> <name0> <player1> <name1> [<option>...]

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
  <option>:
    --stdinLogFile0 <filename>:
    --stdinLogFile1 <filename>:
      When these pathnames are specified, copies of data sent to AI programs
      will be stored.
      These might be useful for debugging AI programs.
    --stderrLogFile0 <filename>:
    --stderrLogFile1 <filename>:
      When these pathnames are specified, the standard error output sent from AI programs
      and our system messages will be stored.
      These might be useful for debugging AI programs.
    --pauseP0 <command>:
    --resumeP0 <command>:
    --pauseP1 <command>:
    --resumeP1 <command>:
      The Game rule says
        "At each step, the game management system suspends the execution of AI
         when receiving its output has been completed, and resumes it after giving per step information".
      These options will implement this, you need not care about them.
    -h [--help]:
      Output help a message.  The content is the same as what is written here.

(2) Course Editor

  Open "editor/editor.html" with a web browser.
  A mannual page can be found by clicking the "Help" button at top right.

(3) Race Viewer

  Open "viewer/viewer.html" with a web browser.
  A mannual page can be found by clicking the "Help" button at top right.

6. Known Bugs and Features Not Implemented (yet)

None

For latest infomations, refer following web site.
  https://github.com/SamurAI-Coding/Software2017-18/issues
If you faced an issue, you can report here or e-mail.
  samurai-contact@ipsj.or.jp
Of course you can send fix from
  https://github.com/SamurAI-Coding/Software2017-18/pulls

Appendix. How to install Boost libraries

Followings show one case of installation on each environments.

(1) Linux

  Run following commands.  Please review and modify it as needed.
    wget -O boost_1_66_0.tar.gz https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz
    tar zxvf boost_1_66_0.tar.gz
    cd boost_1_66_0
    ./bootstrap.sh
    sudo ./b2 install --with-system --with-filesystem --with-program_options --prefix=/usr/local

(2) Mac

  If you installed brew (https://brew.sh/), you can run following commands on terminal to install boost.
    brew update
    brew install boost

  In case you did not install brew and did not want to install brew,
  follow process (1) Linux.

(3) Windows

  (3-1) msys2/mingw*
    If you installed msys2 (http://www.msys2.org/),
    in mingw environment, run
      pacman -Syu
    to update system, and run
      pacman -Ss boost
    to search boost packages.
    In case you use mingw64 on x86_64, you must run
      pacman -S mingw-w64-x86_64-boost
    to install.
    the boost libraries installed on mingw* maybe only those with suffix "-mt" (multi-thread).
    Hence, you may need to run the following command before building our software.
      export BOOST_LIB_SUFFIX=-mt
    This environment variable will be used at `make`.

  (3-2) Visual Studio
    Imagine that we extracted zip archive of latest boost libraries to the following path.
      C:\Users\<User name>\boost\<Boost Version>
        ex) C:\Users\samurai\boost\boost_1_66_0
    Then, we have to register the environment variable `BOOST_ROOT` as the path of above.
    You can confirm the environmnet variable if you run following command on command prompt (cmd.exe).
      echo %BOOST_ROOT%
    Next, open Native tools command prompt.  For example, if we installed Visual Studio 2017 Community Edition,
      "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
    and so on is it.
    Lastly, run following commands on Native tools command prompt.
    Please review and modify it as needed.
      cd %BOOST_ROOT%
      .\bootstrap.bat
      .\b2 install toolset=msvc-14.1 --with-system --with-filesystem --with-program_options --prefix=%BOOST_ROOT%
      move %BOOST_ROOT%\include\boost-1_66\boost %BOOST_ROOT%\include\boost
      rd %BOOST_ROOT%\include\boost-1_66
    You have to specify the version of Visual C++ to `toolset` option.
    About other versions,
      https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
    may be useful.
    After installation, open "SamurAI_Coding_2017-18_Software.sln" in the top level directory.
    It is the solutiuon file to build our software.

