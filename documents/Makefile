TARGET= datafiles.pdf rule-en.pdf rule-jp.pdf
PLATEX= platex
DVI2PDF= dvipdfmx

.SUFFIXES:
.SUFFIXES: .tex .dvi .pdf

all: $(TARGET)

clean:
	rm -f *.dvi *.log *.aux *~

distclean: clean

.tex.dvi:
	$(PLATEX) $<
	$(PLATEX) $<

.dvi.pdf:
	echo $<
	$(DVI2PDF) $<

