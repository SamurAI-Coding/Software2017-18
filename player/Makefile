CXXFLAGS= -pthread -std=c++11 -W -Wall -O2 -march=native
CXX= c++

OBJECTS_GREEDY= greedy.o raceState.o
OBJECTS_PSEUDO_ALPHABETA= pseudo_alphabeta.o raceState.o

TARGET_GREEDY= greedy
TARGET_PSEUDO_ALPHABETA= pseudo_alphabeta

all: ${TARGET_GREEDY} ${TARGET_PSEUDO_ALPHABETA}

${TARGET_GREEDY}: ${OBJECTS_GREEDY}
	${CXX} ${CXXFLAGS} -o $@ $^ ${AM_LDFLAGS}

${TARGET_PSEUDO_ALPHABETA}: ${OBJECTS_PSEUDO_ALPHABETA}
	${CXX} ${CXXFLAGS} -o $@ $^ ${AM_LDFLAGS}

-include ${OBJECTS:.o=.d}

%.o: %.cpp
	${CXX} -c ${CXXFLAGS} $*.cpp -o $*.o
	${CXX} -MM ${CXXFLAGS} $*.cpp > $*.d

clean:
	rm -f *.o *.d
	rm -rf *~ */*~ \#*\#

distclean: clean
	rm -f ${TARGET}
