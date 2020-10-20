#directories containing the headers, libraries and executables
chead:= include
clib := lib
cbin := bin

#flags necessary to the compilation
CC := gcc
CFLAGS:= -fPIC -Wall -Werror -g -I$(chead) -Ilib/cstructures/include
LFLAGS:= -lscreen -lnetwork -ldataset -lalgo -lserialisation -lprotocol
LDFLAGS:= -Wl,--disable-new-dtags -Wl,-rpath,\$$ORIGIN/../lib -Wl,-rpath,\$$ORIGIN/../lib/cstructures/lib -L$(clib) -L$(clib)/cstructures/lib


#executables compilation
client: blib
	@ echo "Building client"
	@ mkdir -p bin
	@ $(CC) $(CFLAGS) $(LDFLAGS) -o $(cbin)/$@ $@.c $(LFLAGS)

server: blib
	@ echo "Builing server"
	@ mkdir -p bin
	@ $(CC) $(CFLAGS) -o $(cbin)/$@ $@.c $(LFLAGS)


.PHONY: blib
blib:
	@ $(MAKE) -f build.mk -C$(clib) all


#overall functions
all: client server

.PHONY: clean
clean:
	@ echo "cleaning binaries"
	@ rm -rf $(cbin)/*
	@ $(MAKE) -f build.mk -C$(clib) clean

