#list of all src and object files
csrc := $(wildcard src/*.c)
cobj := $(csrc:.c=.o)

#directories containing the headers, libraries and executables
chead:= include
clib := lib
cbin := bin

#flags necessary to the compilation
CC ?= gcc
CFLAGS:= -fPIC -Wall -Werror -g -I$(chead)
lib_f:= -lscreen -lnetwork -ldataset -lalgo -lserialisation
lib_b:= libscreen libnetwork libdataset libalgo libserialisation


#executables compilation
client: client.c $(lib_b)
	echo "Building client"
	$(CC) $(CFLAGS) -L$(clib) -o $(cbin)/$@ -c $< $(lib_f) -Wl,-rpath,"$(ORIGIN)$(clib)"

server: server.c $(lib_b)
	echo "Builing server"
	$(CC) $(CFLAGS) -L$(clib) -o $(cbin)/$@ -c $< $(lib_f) -Wl,-rpath,"$(ORIGIN)$(clib)"


#objects compilation from the source files
%.o: %.c
	echo "Building $@"
	$(CC) $(CFLAGS) -o $@ -c $<


#libraries compilation and linking (version number -> *.so file)
libscreen : src/screen.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.so.1 -o $(clib)/$@.so.1.0 $<
	ldconfig -n $(clib)/
	ln -sf $@.so.1 $(clib)/$@.so

libnetwork : src/network.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.so.1 -o $(clib)/$@.so.1.0 $<
	ldconfig -n $(clib)/
	ln -sf $@.so.1 $(clib)/$@.so

libalgo : src/algo.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.so.1 -o $(clib)/$@.so.1.0 $<
	ldconfig -n $(clib)/
	ln -sf $@.so.1 $(clib)/$@.so

libdataset : src/dataset.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.so.1 -o $(clib)/$@.so.1.1 $<
	ldconfig -n $(clib)/
	ln -sf $@.so.1 $(clib)/$@.so

libserialisation : src/serialisation.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.so.1 -o $(clib)/$@.so.1.1 $<
	ldconfig -n $(clib)/
	ln -sf $@.so.1 $(clib)/$@.so


#overall functions
.PHONY: all
all:
	make client
	make server

.PHONY: clean
clean:
	rm -rf $(cobj) $(cbin)/* $(clib)/*
