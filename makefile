#directories containing the headers, libraries and executables
chead:= include
clib := lib
cbin := bin

#flags necessary to the compilation
CC := gcc
CFLAGS:= -fPIC -Wall -Werror -g -I$(chead)
LFLAGS:= -lscreen -lnetwork -ldataset -lalgo -lserialisation -lprotocol
LDFLAGS:= -Wl,--disable-new-dtags -Wl,-rpath,\$$ORIGIN/../lib


#executables compilation
client: client.c blib
	@ echo "Building client"
	$(CC) $(CFLAGS) -L$(clib) $(LDFLAGS) -o $(cbin)/$@ $@.c $(LFLAGS)

server: server.c blib
	@ echo "Builing server"
	$(CC) $(CFLAGS) -L$(clib) $(LDFLAGS) -o $(cbin)/$@ $@.c $(LFLAGS)

test_algo: test_algo.c balgo
	@ echo "Builing $@"
	$(CC) $(CFLAGS) -L$(clib) $(LDFLAGS) -o $(cbin)/$@ $@.c -lalgo -ldataset


.PHONY: blib
blib:
	@ $(MAKE) -f build.mk -C$(clib) all

.PHONY: balgo
balgo:
	@ $(MAKE) -f build.mk -C$(clib) libalgo.so
	@ $(MAKE) -f build.mk -C$(clib) libdataset.so

#overall functions
all: client server

.PHONY: clean
clean:
	@ echo "cleaning binaries"
	@ rm -rf $(cbin)/*
	@ $(MAKE) -f build.mk -C$(clib) clean


