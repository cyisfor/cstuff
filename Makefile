VPATH=src libb64-1.2/src/
P=glib-2.0 gtk+-3.0 gdk-3.0 sqlite3

# make sure pkg-config packages count as system headers to simplify -MMD
CFLAGS=$(subst -I,-isystem ,$(shell pkg-config --cflags $P)) -I. -ggdb3 -Isrc
LDFLAGS=`pkg-config --libs $P`

ALLN:=
O=$(patsubst %,o/%.o,$N $(ALLN)) \
$(foreach name,$N $(ALLN),$(eval objects:=$$(objects) $(name)))

LINK=@echo LINK $@; $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
COMPILE=@echo COMPILE $*; $(CC) -ftabstop=2 -MT $@ -MMD $(CFLAGS) -c -o $@ $<

.PHONY: all clean

all: statements2init search

DBN:=db mmapfile search_schema

N=search_console search tag $(DBN)
search: $(O)
	$(LINK)

N=statements2init $(DBN)
statements2init: $(O)
	$(LINK)

o/%.stmts.sql.c: sql/%.stmts.sql statements2init
	./statements2init <$< >$@.temp
	mv $@.temp $@

o/tag.o: o/tag.stmts.sql.c

o/search.o: o/search.stmts.sql.c

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
