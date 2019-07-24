CXXFLAGS= -pthread -std=c++11 -W -Wall -O2 -march=native
CXX= c++
JAVAC = javac

OBJECTS_GREEDY= greedy.o raceState.o
OBJECTS_PSEUDO_ALPHABETA= pseudo_alphabeta.o raceState.o
OBJECTS_RANDOM_JAVA = GameState.class RandomPlayer.class Point.class
OBJECTS_LOOKAHEAD_JAVA = GameState.class LookaheadPlayer.class Point.class

TARGET_GREEDY= greedy
TARGET_PSEUDO_ALPHABETA= pseudo_alphabeta
TARGET_RANDOM_JAVA = ${OBJECTS_RANDOM_JAVA}
TARGET_LOOKAHEAD_JAVA = ${OBJECTS_LOOKAHEAD_JAVA}

all: ${TARGET_GREEDY} ${TARGET_PSEUDO_ALPHABETA} ${TARGET_RANDOM_JAVA} ${TARGET_LOOKAHEAD_JAVA} 

${TARGET_GREEDY}: ${OBJECTS_GREEDY}
	${CXX} ${CXXFLAGS} -o $@ $^ ${AM_LDFLAGS}

${TARGET_PSEUDO_ALPHABETA}: ${OBJECTS_PSEUDO_ALPHABETA}
	${CXX} ${CXXFLAGS} -o $@ $^ ${AM_LDFLAGS}

-include ${OBJECTS:.o=.d}

%.o: %.cpp
	${CXX} -c ${CXXFLAGS} $*.cpp -o $*.o
	${CXX} -MM ${CXXFLAGS} $*.cpp > $*.d

%.class: %.java
	${JAVAC} $*.java

clean:
	rm -f *.o *.d
	rm -rf *~ */*~ \#*\#
	rm -f *.class

distclean: clean
	rm -f ${TARGET_GREEDY} ${TARGET_PSEUDO_ALPHABETA} 
