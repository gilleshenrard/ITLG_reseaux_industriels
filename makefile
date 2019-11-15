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
lib_b:= libscreen.so libnetwork.so libdataset.so libalgo.so libserialisation.so


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
libscreen.so : src/screen.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $(clib)/$@.1.0 $<
	ldconfig -n $(clib)/
	ln -sf $@.1 $(clib)/$@

libnetwork.so : src/network.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $(clib)/$@.1.0 $<
	ldconfig -n $(clib)/
	ln -sf $@.1 $(clib)/$@

libalgo.so : src/algo.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $(clib)/$@.1.0 $<
	ldconfig -n $(clib)/
	ln -sf $@.1 $(clib)/$@

libdataset.so : src/dataset.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $(clib)/$@.1.1 $<
	ldconfig -n $(clib)/
	ln -sf $@.1 $(clib)/$@

libserialisation.so : src/serialisation.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $(clib)/$@.1.1 $<
	ldconfig -n $(clib)/
	ln -sf $@.1 $(clib)/$@


#overall functions
all: client server

.PHONY: clean
clean:
	rm -rf $(cobj) $(cbin)/* $(clib)/*
