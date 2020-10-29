#list of all src and object files
csrc := $(wildcard ../src/*.c)
cobj := $(csrc:.c=.o)

#directories containing the headers, libraries and executables
chead:= ../include

#flags necessary to the compilation
CC := gcc
CFLAGS:= -fPIC -Wall -Werror -Wextra -g -I$(chead) -Icstructures/include
lib_b:= libscreen.so libnetwork.so libdataset.so libserialisation.so libprotocol.so balgo

#objects compilation from the source files
%.o: %.c
	@ echo "Building $@"
	@ $(CC) $(CFLAGS) -o $@ -c $<


#libraries compilation and linking (version number -> *.so file)
libscreen.so : ../src/screen.o
	@ echo "Building $@"
	@ $(CC) -shared -fPIC -lc -Wl,-soname,$@.1 -o $@.1.0 $<
	@ ldconfig -n . -l $@.1.0
	@ ln -sf $@.1 $@

libnetwork.so : ../src/network.o
	@ echo "Building $@"
	@ $(CC) -shared -fPIC -lc -Wl,-soname,$@.2 -o $@.2.0 $<
	@ ldconfig -n . -l $@.2.0
	@ ln -sf $@.2 $@

libdataset.so : ../src/dataset.o
	@ echo "Building $@"
	@ $(CC) -shared -fPIC -lc -Wl,-soname,$@.1 -o $@.1.2 $<
	@ ldconfig -n . -l $@.1.2
	@ ln -sf $@.1 $@

libserialisation.so : ../src/serialisation.o
	@ echo "Building $@"
	@ $(CC) -shared -fPIC -lc -Wl,-soname,$@.1 -o $@.1.1 $<
	@ ldconfig -n . -l $@.1.1
	@ ln -sf $@.1 $@

libprotocol.so : ../src/protocol.o balgo libnetwork.so libserialisation.so
	@ echo "Building $@"
	@ $(CC) -shared -fPIC -lc -L. -Lcstructures/lib -Wl,-soname,$@.2 -o $@.2.0 $< -lalgo -lnetwork -lserialisation
	@ ldconfig -n . -l $@.2.0
	@ ln -sf $@.2 $@


#overall functions
all: $(lib_b)

#phony rules to build needed libraries and to clean builds
.PHONY: balgo
balgo:
	@ $(MAKE) -f makefile -Ccstructures balgo

.PHONY= clean
clean:
	@ echo "cleaning libraries *.so files"
	@ rm -rf *.so* ../src/*.o
	@ $(MAKE) -f makefile -Ccstructures clean
