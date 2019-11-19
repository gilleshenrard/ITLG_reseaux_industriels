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
client: client.c blib $(lib_b)
	echo "Building client"
	$(CC) $(CFLAGS) -L$(clib) -o $(cbin)/$@ $@.c $(lib_f) -Wl,-rpath,"$(ORIGIN)$(clib)"

server: server.c blib $(lib_b)
	echo "Builing server"
	$(CC) $(CFLAGS) -L$(clib) -o $(cbin)/$@ $@.c $(lib_f) -Wl,-rpath,"$(ORIGIN)$(clib)"

test_algo: test_algo.c balgo $(clib)/libdataset.so $(clib)/libalgo.so
	echo "Builing $@"
	$(CC) $(CFLAGS) -L$(clib) -o $(cbin)/$@ $@.c -lalgo -ldataset -Wl,-rpath,"$(ORIGIN)$(clib)"


.PHONY: blib
blib:
	$(MAKE) -f build.mk -C lib all

.PHONY: balgo
balgo:
	$(MAKE) -f build.mk -C lib libalgo.so
	$(MAKE) -f build.mk -C lib libdataset.so

#overall functions
all: client server

.PHONY: clean
clean:
	rm -rf $(cbin)/*
	$(MAKE) -f build.mk -C lib clean


