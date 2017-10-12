CXXFLAGS= -pthread -std=c++11 -g -Wall
CXX= c++

OBJECTS= main.o course.o raceState.o player.o
SOURCES= *.cpp *.hpp

TARGET= official

all: ${TARGET}

tags: ${SOURCES}
	etags $^

${TARGET}: ${OBJECTS}
	${CXX} ${CXXFLAGS} -o $@ $^

-include ${OBJECTS:.o=.d}

%.o: %.cpp
	${CXX} -c ${CXXFLAGS} $*.cpp -o $*.o
	${CXX} -MM ${CXXFLAGS} $*.cpp > $*.d

clean:
	rm -f TAGS
	rm -f *.o *.d
	rm -rf *~ */*~ \#*\#

distclean: clean
	rm -f ${TARGET}
