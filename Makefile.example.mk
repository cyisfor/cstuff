all: main

# make will turn %.c into src/%.c this way but ONLY in dependencies, not in targets
VPATH=src

# pkg-config stuff
P=
export PKG_CONFIG_PATH=
# make sure pkg-config packages count as system headers to simplify -MMD
CFLAGS+=$(subst -I,-isystem ,$(shell pkg-config --cflags $P))
LDLIBS+=`pkg-config --libs $P`

# debugging eh, switch -ggdb for -O2 when finished debugging
# I don't recommend -g -O2 because it makes stepping through your code like...
# "what? optimized away? aw man!"
# "wait, why is it repeating the same two lines like 3 times?"
# "oh hey, after the second time suddenly the variable's not optimized away!"
# "Is stepping through really going to repeat every 2 lines? It is, isn't it?"
# "I just need to see what this function returns AH DAMMIT IT WENT UP 6 RETURNS"
CFLAGS+=-ggdb
CFLAGS+=-ftabstop=2 -fdiagnostics-color=auto

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
# this is a really sneaky trick, so I'll explain
# thanks to Tom Tromney I guess for this trick (whoever he is)
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

o/%.o: %.c o/%.d | o
	$(COMPILE)

o/%.d: | o ;

# ---

# since all o/%.d files have an empty recipe, with no (non-order-only)
# prerequisites, then if o/something.d file exists, then make o/something.d is a no-op, and
# o/something.d is always up-to-date, so o/something.o never gets compiled on its account.

# but if o/something.d doesn't exist... it's considered out-of-date!
# since it's not .PHONY, it's considered up-to-date if it exists, but if not,
# then o/%.o has an out-of-date dependency, and needs to be rebuilt!

# rebuilding .o causes .d to be created as a side effect, even though make has
# no idea this is the case. Further invocations of make will find .d existing just fine, and
# thus will only rebuild .o if .c changes. make will consider the old .d to be up-to-date,
# and will never update it... but gcc will.

# so every time the .c changes, make knows to update the .o, and has no idea that .d
# is out-of-date, but in updating the .o, gcc also updates the .d ensuring that it isn't
# out-of-date. Thus the .d always remains up-to-date, and is created if it doesn't exist

# Thus, if your compilation produces multiple files, they should NOT be multiple targets
# instead, there should be one target for the main generated file, and that target should
# depend on the source, and also the other generated files, which should have empty recipes.

# ---

# don't forget to add an order-only prerequisite for everything generating files in o
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

# don't include (generate) .d/.o files if we're cleaning, please.
ifneq ($(MAKECMDGOALS),clean)
# include all the dependencies for the modules found so far via $(N) / $(O)
-include $(patsubst %, o/%.d,$(mods))
endif

.PHONY: all clean
