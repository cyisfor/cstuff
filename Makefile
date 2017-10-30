all: main

# make will turn %.c into src/%.c this way but ONLY in dependencies, not in targets
VPATH=src

# pkg-config stuff
P=
export PKG_CONFIG_PATH=
# make sure pkg-config packages count as system headers to simplify -MMD
CFLAGS+=$(subst -I,-isystem ,$(shell pkg-config --cflags $P))
LDLIBS+=`pkg-config --libs $P`

# debugging eh
CFLAGS+=-ggdb3
CFLAGS+=-ftabstop=2 -fdiagnostic-colors=auto

# any includes that are target specific, just set INC=
CFLAGS+=$(patsubst %,-I%,$(INC))
INC:=.

# modules common to all targets
ALLN:=commonmodule

# switch all names for object names, and add those names to the list of module names compiled
# since this is lazy, $N will be different if we assign it, then access $(O)
O=$(patsubst %,o/%.o,$N $(ALLN)) \
$(foreach name,$N $(ALLN),$(eval mods:=$$(mods) $(name)))

# shared objects should be .os... actually we should use libtool for everything but I don't
# understand it.
OS=$(patsubst %,o/%.os,$N $(ALLN)) \
$(foreach name,$N $(ALLN),$(eval mods:=$$(mods) $(name)))

# generate stuff like programs, libraries, and object files
# since these are lazy, will handle any target specific CFLAGS or w/ev (like INC)
PROGRAM=@echo PROGRAM $@; $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
LIBRARY=@echo LIBRARY $@; $(CC) -shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
# also regenerate the dependency file, because we might as well, saves parsing twice
COMPILE=@echo COMPILE $*; $(CC) -MT $@ -MMD $(CFLAGS) -c -o $@ $<

# the main rule section, where programs are generated.
N=main
main: $(O)
	$(PROGRAM)

# to generate stuff, make the generated file depend on the generator
# be sure not to create $@ until absolutely sure it's legit, or make
# will accept an empty/corrupt one and not think to rebuild it
o/%.generate.c: sql/%.generate generator
	./generator <$< >$@.temp
	mv $@.temp $@

# this is a common packing thing I like to use...
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

# turns data into .c files, then you #include o/$(whatever).pack.c

# generate objects, and also update .d files
o/%.o: %.c | o
	$(COMPILE)

# since above regenerates .d, only need this rule when .d doesn't exist
# technically we could have the above use targets o/%.d and o/%.o but... that ends up
# acting weird... and making $@ pretty much worthless

o/%.d: | %.c o
	@echo DEP $*; $(CC) -ftabstop=2 -MT o/$*.o -MM -MG $(CFLAGS) -c -o $@ $(firstword $|)

# be sure that everything generating files in o depends on o (order only)
o:
	mkdir o

# fancy schmancy!
# don't ever blindly git clean -fdx unless you want to cry when you forgot to add
# that source file you worked on for three hours to the repository.

clean:
	git clean -ndx
	@echo ^C to not delete
	@read
	git clean -fdx
	(cd data_to_header_string; exec ninja -t clean)

ifneq ($(MAKECMDGOALS),clean)
# include all the dependencies for the modules found so far via $(N) / $(O)
-include $(patsubst %, o/%.d,$(mods))
endif

.PHONY: all clean
