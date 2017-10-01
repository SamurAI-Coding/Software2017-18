all: DOCUMENTS OFFICIAL PLAYER VIEWER EDITOR

DOCUMENTS:
	cd documents; make

OFFICIAL:
	cd official; make

PLAYER:
	cd player; make

VIEWER:

EDITOR:

tags:
	etags */*.hpp */*.cpp

tar: distclean
	tar zcvf ../samurai-software-`date -Iminutes` *

clean:
	rm -rf *~ */*~ \#*\#
	cd documents; make clean
	cd official; make clean
	cd player; make clean
	cd viewer; make clean
	cd editor; make clean

distclean: clean
	cd documents; make distclean
	cd official; make distclean
	cd player; make distclean
	cd viewer; make distclean
	cd editor; make distclean
