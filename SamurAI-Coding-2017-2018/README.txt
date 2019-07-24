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
  datafiles.pdf: Description of data file formats (in Japanese)

4. Installation

To build the execution management system and sample players,
run "make" in the corresponding directories.
A C++ language system supporting C++11 and the Boost library are required.

The Boost library must be Version 1.64.0 or newer.
Check the version of Boost if you install it using package manager.
Especially, apt (Advanced Package Tool) on Ubuntu
cannot install Boost of the required version from their standard package archives.
Please get and install Boost manually referring to the following website.
  http://www.boost.org/doc/libs/1_66_0/more/getting_started/index.html
You can choose full installation, but our game manager only requires
  system, filesystem, program_options
libraries.  So, the minimum configuration is to run b2 program with the following options:
  --with-system --with-filesystem --with-program_options
Please refer to Appendix of this file for details.

We also distribute a game manager to which the Boost libraries are statically linked
(experimental) in the following GitHub Release page:
  https://github.com/SamurAI-Coding/Software2017-18/releases/latest
You need not install Boost if you use this.

For the viewer and the editor, no installation operation is necessary.
They can be executed on a web browser supporting Javascript.

5. Usage

(1) Execution Management System

The following command line arguments should be specified.
　　./official <course file> <player0> <name0> <player1> <name1> [<option>...]

  <course file>:
    File describing the course to be used in the race.
    This file is usually output by clicking the "Write" button on the course editor.
    "sample/sample-course.smrjky" is a sample of a course file.
  <player0>, <player1>:
    Pathnames of executables of AI programs to compete in the race.
    A sample player executable can be built as "player/greedy"
    by running "make" in the "player" directory.
  <name0>, <name1>:
    Names of AI programs to be used in the race viewer.
  <option>:
    --stdinLogFile0 <filename>:
    --stdinLogFile1 <filename>:
      When these pathnames are specified, copies of data sent to AI programs
      will be stored.
      These may be useful for debugging AI programs.
    --stderrLogFile0 <filename>:
    --stderrLogFile1 <filename>:
      When these pathnames are specified, the standard error output of AI programs
      and our system messages will be stored.
      These may be useful for debugging AI programs.
    --pauseP0 <command>:
    --resumeP0 <command>:
    --pauseP1 <command>:
    --resumeP1 <command>:
      The Game Rule says
        "At each step, the game management system suspends the execution of AI
         when receiving its output has been completed, and resumes it after giving per step information".
      These options are used to perform this. You need not specify these options under normal use.
    -h [--help]:
      Output the help message.  The content is almost the same to this document.

(2) Course Editor

  Open "editor/editor.html" with a web browser.
  A manual page can be shown by clicking the "Help" button on the top right.

(3) Race Viewer

  Open "viewer/viewer.html" with a web browser.
  A manual page can be shown by clicking the "Help" button on the top right.

6. Known Bugs and Features Not Implemented yet

None

Latest information:
  https://github.com/SamurAI-Coding/Software2017-18/issues
When you faced any issue, please report from this website or by e-mail:
  samurai-contact@ipsj.or.jp
Of course you can send a fix from
  https://github.com/SamurAI-Coding/Software2017-18/pulls

Appendix: How to install the Boost libraries

The followings show examples of installation operations on several platforms.

(1) Linux

  Run the following commands (Please modify them as necessary).
    wget -O boost_1_66_0.tar.gz https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz
    tar zxvf boost_1_66_0.tar.gz
    cd boost_1_66_0
    ./bootstrap.sh
    sudo ./b2 install --with-system --with-filesystem --with-program_options --prefix=/usr/local

(2) Mac

  If you have installed brew (https://brew.sh/), you can run following commands on terminal to install boost.
    brew update
    brew install boost

  In case you have not installed and do not want to install brew,
  follow the process written in (1) Linux.

(3) Windows

  (3-1) msys2/mingw*
    If you have installed msys2 (http://www.msys2.org/),
    in mingw environment, run
      pacman -Syu
    to update system, and run
      pacman -Ss boost
    to search boost packages.
    In case you use mingw64 on x86_64, you should run
      pacman -S mingw-w64-x86_64-boost
    to install.
    The Boost libraries installed on mingw* seem to be only those with suffix "-mt" (multi-thread).
    Hence, you may need to run the following command before building our software.
      export BOOST_LIB_SUFFIX=-mt
    This environment variable will be used in the "make" process.

  (3-2) Visual Studio
    Suppose that the zip archive of latest Boost libraries is extracted to the following path:
      C:\Users\<User name>\boost\<Boost Version>
      (e.g., C:\Users\samurai\boost\boost_1_66_0)
    Then, you need to register the environment variable `BOOST_ROOT` as the path written above.
    You can check this environment variable by the following command on a command prompt (cmd.exe).
      echo %BOOST_ROOT%
    Next, open a Native tools command prompt.
    For example, if you have installed Visual Studio 2017 Community Edition, execute
      "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
    Lastly, run the following commands on the Native tools command prompt (Please modify them as necessary).
      cd %BOOST_ROOT%
      .\bootstrap.bat
      .\b2 install toolset=msvc-14.1 --with-system --with-filesystem --with-program_options --prefix=%BOOST_ROOT%
      move %BOOST_ROOT%\include\boost-1_66\boost %BOOST_ROOT%\include\boost
      rd %BOOST_ROOT%\include\boost-1_66
    Note that you need to specify the version of Visual C++ to the "toolset" option above.
    For specifying the versions other than msvc-14.1,
      https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
    will be helpful.
    After installation, open "SamurAI_Coding_2017-18_Software.sln" in the top level directory.
    This is the solution file to build our software.
