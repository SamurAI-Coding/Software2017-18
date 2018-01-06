SamurAI Jockey ゲーム用ソフトウェアについて

                                                            2017/12/30

このディレクトリには SamurAI Coding 2017-18 で用いるために準備中の
SamurAI Jockey ゲーム用のソフトウェアが入っています。

1. ディレクトリ構成

トップレベルにはこの README-jp.txt の他に下記のサブディレクトリがあります。
  documents:
    ゲームのルールやデータファイルの形式についての文書が入っています。
  official:
    レースの実行管理システムが入っています。
  viewer:
    レースのログをビジュアライズするビューワのソフトウェアが入っています。
  editor:
    ゲームに用いるコースを編集するためのソフトウェアが入っています。
  player:
    ゲームを競うプレイヤソフトウェアのサンプルが入っています。
  samples:
    レースコースやレースのログのサンプルが入っています。

2. サンプルレースを見るには

viewer/viewer.html をブラウザで開きます。
左上端の Load Log ボタンを押すとダイアログが表示されますので、
ここでこのディレクトリ中の samples/sample.racelog を選んでください。
"Start" ボタンを押すとレースの進行を見ることができます。
詳しい使い方については "Help" ボタンを押してマニュアルページを見てください。

3. 文書

documents ディレクトリには以下の文書が入っています。

  rule-jp.pdf: ゲームのルール
  rule-en.pdf: ゲームのルール (英語版)
  datafiles.pdf: 情報交換に用いるファイルの形式

4. インストール

実行管理システムとサンプルプレイヤについては、各ディレクトリで make を走らせてください。
インストールには C++11 対応の C++ 言語システムと boost ライブラリが必要です。

boost ライブラリについてはバージョン 1.64.0 以上が必要です。
パッケージマネージャでインストールする際はそのバージョンについて確認してください。
特に Ubuntu の apt : Advanced Package Tool では
標準パッケージアーカイブからインストールができません。

ビルド・インストール方法は下記サイトが参考になります。
  https://boostjp.github.io/howtobuild.html
フルインストールするのも選択肢の一つですが、
実行管理システムを実行するには
  system, filesystem, program_options
ライブラリをインストールする必要があるため、 b2 プログラムに
  --with-system --with-filesystem --with-program_options
オプションを付けてビルド・インストールすると最小構成になります。
詳細は本ファイル下部の「付録」をご覧ください。

試験的に boost ライブラリを静的リンクした実行管理システムのバイナリを
下記 GitHub Release ページにて配布しておりますのでご活用ください。
  https://github.com/SamurAI-Coding/Software2017-18/releases/latest
これを利用する際には boost のインストールは必要ありません。

ビューワとエディタについては特段のインストールは不要で、
Javascript を利用できるウェブブラウザーならすぐ実行できます。

5. 使い方

(1) 実行管理システム

コマンドラインに以下のように引数を指定します。
  ./official <course file> <player0> <name0> <player1> <name1> [<option>...]
各引数は以下のとおりです。

  <course file>:
    レースで用いる形式のコース記述ファイル。
    通常 editor でコースを設計して、Write で書き出したものです。
    サンプルが sample/sample-course.smrjky にあります。
  <player0>, <player1>:
    レースを競う AI プログラムの実行可能形式ファイルのパス名です。
    player ディレクトリで make すれば、サンプルが player/greedy にできます。
  <name0>, <name1>:
    Viewer の表示に使う AI プログラムの名前の文字列です。
  <option>:
    --stdinLogFile0 <filename>:
    --stdinLogFile1 <filename>:
      これらのパス名を指定すると、対象の AI プログラムに送ったデータのコピーを指定した格納します。
      これらを AI プレイヤの入力とすれば、デバッグに便利でしょう。
    --stderrLogFile0 <filename>:
    --stderrLogFile1 <filename>:
      これらのパス名を指定すると、対象の AI プログラムが出力した標準エラー出力のコピーと
      システムメッセージが指定したファイルに格納されます。
      デバッグにご活用ください。
    --pauseP0 <command>:
    --resumeP0 <command>:
    --pauseP1 <command>:
    --resumeP1 <command>:
      ルールに記載されているように、「ゲーム管理システムは, 各ステップについて出力を終了した
      AI の動作を一時停止し, 次ステップの情報を与える際に動作を再開させ」ます。
      これを実現するためのオプションとなっており、通常はこのオプションを利用する必要がありません。
    -h [--help]:
      ヘルプメッセージを出力します。内容はここで書いてあることと同じです。

例えばビルドの済んだトップディレクトリで

  ./official/official samples/sample-course.smrjky player/greedy 0 player/greedy 1 --stdinLogFile0 stdin0.txt --stdinLogFile1 stdin1.txt --stderrLogFile0 stderr0.txt --stderrLogFile1 stderr1.txt > gamelog.json

を実行すると
  stdin0.txt : player0 に与えられた入力
  stdin1.txt : player1 に与えられた入力
  stderr0.txt : player0 の標準エラー出力とシステムメッセージ
  stderr1.txt : player1 の標準エラー出力とシステムメッセージ
  gamelog.json : ゲームのログ(後述の(3) レースビューワでその内容を確認できます)
が得られます。

(2) コース編集ソフトウェア

  editor/editor.html をブラウザで開きます。
  右上端の Help ボタンを押して編集方法の詳細をご覧ください。

(3) レースビューワ

  viewer/viewer.html をブラウザで開きます。
  右上端の Help ボタンを押して使い方の詳細をご覧ください。

6. 既知のバグ・未実装機能

なし

最新の情報については
  https://github.com/SamurAI-Coding/Software2017-18/issues
で確認することができます。
何か問題が発生したらこちらに連絡いただくか
  samurai-contact@ipsj.or.jp
宛にメールください。
もちろん、
  https://github.com/SamurAI-Coding/Software2017-18/pulls
から修正を送っていただいても構いません。

付録. boost のビルド・インストール

各環境におけるビルド・インストール方法の一例を紹介します。

(1) Linux

  下記コマンドを実行します。必要に応じて変更してください。
    wget -O boost_1_65_1.tar.gz https://dl.bintray.com/boostorg/release/1.65.1/source/boost_1_65_1.tar.gz
    tar zxvf boost_1_65_1.tar.gz
    cd boost_1_65_1
    ./bootstrap.sh
    sudo ./b2 install --with-system --with-filesystem --with-program_options --prefix=/usr/local

(2) Mac

  brew (https://brew.sh/index_ja.html) をインストールされていたら
    brew update
    brew install boost
  でインストールできます。

  brew をインストールしておらず、brew をインストールしたくない方は
  (1) Linux の方法に準じてください。

(3) Windows

  (3-1) msys2/mingw*
    msys2 (http://www.msys2.org/) をインストールされていたら、
    mingw 環境において
      pacman -Syu
    をしてシステムを更新した後、
      pacman -Ss boost
    と検索して該当するパッケージをインストールします。
    例えば、x86_64 の msys2 をインストールしていて mingw64 環境を利用している場合
      pacman -S mingw-w64-x86_64-boost
    とします。
    インストールされる boost ライブラリは -mt (multi-thread) という接尾辞がついているもののみになるようです。
    実行管理システムをビルドする際には事前に
      export BOOST_LIB_SUFFIX=-mt
    というコマンドを実行してください。この環境変数は make 時に利用されます。

  (3-2) Visual Studio
    boost ライブラリの最新バージョンの zip アーカイブを
      C:\Users\<User name>\boost\<Boost Version>
        ex) C:\Users\samurai\boost\boost_1_65_1
    に解凍したとします。このとき C:\Users\<User name>\boost\<Boost Version> を BOOST_ROOT として環境変数に登録してください。
    次に Native Tools コマンドプロンプトを開いてください。例えば Visual Studio 2017 Community Edition をインストールしていたら
      "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
    などがそれに該当します。
    以下のコマンドを実行します。下記コマンドは Visual Studio 2017 がインストールされた環境に合わせています。
      cd %BOOST_ROOT%
      .\bootstrap.bat
      .\b2 install toolset=msvc-14.1 --with-system --with-filesystem --with-program_options --prefix=%BOOST_ROOT%
      move %BOOST_ROOT%\include\boost-1_65\boost %BOOST_ROOT%\include\boost
      rd %BOOST_ROOT%\include\boost-1_65
    toolset には Visual C++ のバージョンを指定します。
    他バージョンについては
      https://ja.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B#%E8%A3%BD%E5%93%81%E3%83%90%E3%83%BC%E3%82%B8%E3%83%A7%E3%83%B3%E3%81%A8%E5%86%85%E9%83%A8%E3%83%90%E3%83%BC%E3%82%B8%E3%83%A7%E3%83%B3
    が参考になるかと思います。
    インストールが終了したら、トップレベルの SamurAI_Coding_2017-18_Software.sln を開くとソフトウエアをビルドするためのソリューションが開かれます。


以上
