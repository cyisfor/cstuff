<<<<<<< HEAD
VPATH=src
P=

export PKG_CONFIG_PATH=

CFLAGS+=-ggdb3
# make sure pkg-config packages count as system headers to simplify -MMD
CFLAGS+=$(subst -I,-isystem ,$(shell pkg-config --cflags $P))
CFLAGS+=-ftabstop=2 -fdiagnostic-colors=auto
LDFLAGS+=`pkg-config --libs $P`

CFLAGS+=$(patsubst %,-I%,$(INC))

INC:=.
ALLN:=common

O=$(patsubst %,o/%.o,$N $(ALLN)) \
$(foreach name,$N $(ALLN),$(eval objects:=$$(objects) $(name)))

OS=$(patsubst %,o/%.os,$N $(ALLN)) \
$(foreach name,$N $(ALLN),$(eval objects:=$$(objects) $(name)))

EXE=@echo EXE $@; $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
LIBRARY=@echo LIBRARY $@; $(CC) -shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
COMPILE=@echo COMPILE $*; $(CC) -MT $@ -MMD $(CFLAGS) -c -o $@ $<

.PHONY: all clean

all: main

N=main
main: $(O)
	$(EXE)

o/%.generate.c: sql/%.generate generator
	./generator <$< >$@.temp
	mv $@.temp $@

data_to_header_string/pack: | data_to_header_string
	cd data_to_header_string && ninja

define PACK
generated: o/$F.pack.c
o/$(dir $F): | o
	mkdir $$@
o/$F.pack.c: $F data_to_header_string/pack | o o/$(dir $F)
	@echo PACK $F $N
	@name=$N ./data_to_header_string/pack < $F >$$@.temp
	@mv $$@.temp $$@
endef

N=schema
F=sql/search_schema.sql
$(eval $(PACK))

N=schema
F=sql/schema.sql
$(eval $(PACK))

o/search_schema.o: o/sql/search_schema.sql.pack.c

o/db.o: o/sql/schema.sql.pack.c

#N=uiData
#F=ui.xml
#$(eval $(PACK))

o/%.o: %.c | o
	$(COMPILE)

o/%.d: %.c | o
	@echo DEP $*; $(CC) -ftabstop=2 -MT o/$*.o -MM -MG $(CFLAGS) -c -o $@ $<

o:
	mkdir o

clean:
	git clean -ndx
	@echo ^C to not delete
	@read
	git clean -fdx
	(cd data_to_header_string; exec ninja -t clean)

-include $(patsubst %, o/%.d,$(objects))

data_to_header_string:
	git clone ~/code/data_to_header_string
=======
VPATH=src
P=libxml-2.0
CFLAGS+=$(shell pkg-config --cflags $(P))
LDLIBS+=$(shell pkg-config --libs $(P))
LDLIBS+=-lpcre
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
>>>>>>> c0cb6d6e7ca0fecf76e0a9a34f442f9a1ca3bd0b
