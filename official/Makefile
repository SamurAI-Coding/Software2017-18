CPPFLAGS= -pthread -std=c++11 -g -Wall
CPP= c++

OBJECTS= main.o course.o raceState.o player.o
SOURCES= *.cpp *.hpp

TARGET= official

all: ${TARGET}

tags: ${SOURCES}
	etags $^

${TARGET}: ${OBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

-include ${OBJECTS:.o=.d}

%.o: %.cpp
	${CPP} -c ${CPPFLAGS} $*.cpp -o $*.o
	${CPP} -MM ${CPPFLAGS} $*.cpp > $*.d

clean:
	rm -f TAGS
	rm -f *.o *.d
	rm -rf *~ */*~ \#*\#

distclean: clean
	rm -f ${TARGET}
