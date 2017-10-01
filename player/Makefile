CPPFLAGS= -pthread -std=c++11 -g -Wall
CPP= c++

OBJECTS= greedy.o raceState.o

TARGET= greedy

all: ${TARGET}

${TARGET}: ${OBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

-include ${OBJECTS:.o=.d}

%.o: %.cpp
	${CPP} -c ${CPPFLAGS} $*.cpp -o $*.o
	${CPP} -MM ${CPPFLAGS} $*.cpp > $*.d

clean:
	rm -f *.o *.d
	rm -rf *~ */*~ \#*\#

distclean: clean
	rm -f ${TARGET}
