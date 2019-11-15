#list of all src and object files
csrc := $(wildcard src/*.c)
cobj := $(csrc:.c=.o)

#directories containing the headers, libraries and executables
chead:= include
clib := lib
cbin := bin

#flags necessary to the compilation
CC := gcc
CFLAGS:= -fPIC -Wall -Werror -g -I$(chead)
lib_f:= -lscreen -lnetwork -ldataset -lalgo -lserialisation
lib_b:= $(clib)/libscreen.so $(clib)/libnetwork.so $(clib)/libdataset.so $(clib)/libalgo.so $(clib)/libserialisation.so


#executables compilation
client: client.c $(lib_b) blib
	echo "Building client"
	$(CC) $(CFLAGS) -L$(clib) -o $(cbin)/$@ $@.c $(lib_f) -Wl,-rpath,"$(ORIGIN)$(clib)"

server: server.c $(lib_b) blibb
	echo "Builing server"
	$(CC) $(CFLAGS) -L$(clib) -o $(cbin)/$@ $@.c $(lib_f) -Wl,-rpath,"$(ORIGIN)$(clib)"


#objects compilation from the source files
%.o: %.c
	echo "Building $@"
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: blib
blib:
	$(MAKE) -f lib/build.mk

#overall functions
all: client server

.PHONY: clean
clean:
	rm -rf $(cobj) $(cbin)/* $(clib)/*.so*
