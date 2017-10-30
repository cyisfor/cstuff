all: main

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
ALLN:=commonmodule

O=$(patsubst %,o/%.o,$N $(ALLN)) \
$(foreach name,$N $(ALLN),$(eval objects:=$$(objects) $(name)))

OS=$(patsubst %,o/%.os,$N $(ALLN)) \
$(foreach name,$N $(ALLN),$(eval objects:=$$(objects) $(name)))

EXE=@echo EXE $@; $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
LIBRARY=@echo LIBRARY $@; $(CC) -shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
COMPILE=@echo COMPILE $*; $(CC) -MT $@ -MMD $(CFLAGS) -c -o $@ $<

N=main
main: $(O)
	$(EXE)

o/%.generate.c: sql/%.generate generator
	./generator <$< >$@.temp
	mv $@.temp $@

data_to_header_string/pack: | data_to_header_string
	$(MAKE) -C data_to_header_string

data_to_header_string:
	git clone ~/code/data_to_header_string

define PACK
generated: o/$F.pack.c
o/$(dir $F): | o
	mkdir $$@
o/$F.pack.c: $F data_to_header_string/pack | o o/$(dir $F)
	@echo PACK $F $N
	@name=$N ./data_to_header_string/pack < $F >$$@.temp
	@mv $$@.temp $$@
endef

N=name
F=some/file.dat
$(eval $(PACK))

o/%.o: %.c | o
	$(COMPILE)

# since compiling regenerates .d, only need this rule when .d doesn't exist
o/%.d: | %.c o
	@echo DEP $*; $(CC) -ftabstop=2 -MT o/$*.o -MM -MG $(CFLAGS) -c -o $@ $(firstword $|)

o:
	mkdir o

clean:
	git clean -ndx
	@echo ^C to not delete
	@read
	git clean -fdx
	(cd data_to_header_string; exec ninja -t clean)

-include $(patsubst %, o/%.d,$(objects))

.PHONY: all clean

