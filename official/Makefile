CXXFLAGS= -pthread -std=c++11 -W -Wall -g -ggdb
AM_LDFLAGS= -lboost_system$(BOOST_LIB_SUFFIX) -lboost_filesystem$(BOOST_LIB_SUFFIX) -lboost_program_options$(BOOST_LIB_SUFFIX)
CXX= c++

OBJECTS= main.o course.o raceState.o player.o
SOURCES= *.cpp *.hpp

TARGET= official

ifeq ($(OS), Windows_NT)
AM_LDFLAGS += -lws2_32 -static
endif

all: ${TARGET}

tags: ${SOURCES}
	etags $^

${TARGET}: ${OBJECTS}
ifeq ($(OS), Windows_NT)
ifndef BOOST_LIB_SUFFIX
	@echo "You didn't specify variable BOOST_LIB_SUFFIX."
	@echo "If you build in mingw64, it might be a solution to set \`BOOST_LIB_SUFFIX=-mt\`."
else
	@echo "You specified variable BOOST_LIB_SUFFIX=$(BOOST_LIB_SUFFIX)"
	@echo "If it doesn't run to build, you should \`make clean\`."
endif
endif
	${CXX} ${CXXFLAGS} -o $@ $^ ${AM_LDFLAGS}

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
