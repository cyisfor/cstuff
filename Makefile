VPATH=src
P=libxml-2.0
CFLAGS+=$(shell pkg-config --cflags $(P))
LDLIBS+=$(shell pkg-config --libs $(P))
CFLAGS+=-Ilibxmlfixes
CFLAGS+=-ggdb

all: main

LINK=$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
O=$(patsubst %,o/%.o,$(N))

N=main wordcount
main: $(O) 
	$(LINK) libxmlfixes/libxmlfixes.a

o/main.o: libxmlfixescheck

libxmlfixescheck: libxmlfixes
	$(MAKE) -C libxmlfixes

libxmlfixes:
	git clone ~/code/libxmlfixes/


o/%.o: %.c | o
	$(CC) $(CFLAGS) -c -o $@ $<

o:
	mkdir $@
