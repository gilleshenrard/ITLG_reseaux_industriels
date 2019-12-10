#list of all src and object files
csrc := $(wildcard ../src/*.c)
cobj := $(csrc:.c=.o)

#directories containing the headers, libraries and executables
chead:= ../include

#flags necessary to the compilation
CC := gcc
CFLAGS:= -fPIC -Wall -Werror -g -I$(chead)
lib_b:= libscreen.so libnetwork.so libdataset.so libalgo.so libserialisation.so libprotocol.so

#objects compilation from the source files
%.o: %.c
	@ echo "Building $@"
	$(CC) $(CFLAGS) -o $@ -c $<


#libraries compilation and linking (version number -> *.so file)
libscreen.so : ../src/screen.o
	@ echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.0 $<
	ldconfig -l $@.1.0
	ln -sf $@.1 $@

libnetwork.so : ../src/network.o
	@ echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.2 -o $@.2.0 $<
	ldconfig -l $@.2.0
	ln -sf $@.2 $@

libalgo.so : ../src/algo.o
	@ echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.2 -o $@.2.0 $<
	ldconfig -l $@.2.0
	ln -sf $@.2 $@

libdataset.so : ../src/dataset.o
	@ echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.2 $<
	ldconfig -l $@.1.2
	ln -sf $@.1 $@

libserialisation.so : ../src/serialisation.o
	@ echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.1 $<
	ldconfig -l $@.1.1
	ln -sf $@.1 $@

libprotocol.so : ../src/protocol.o libalgo.so libnetwork.so libserialisation.so
	@ echo "Building $@"
	$(CC) -shared -L. -Wl,-soname,$@.1 -o $@.1.0 $< -lalgo -lnetwork -lserialisation
	ldconfig -l $@.1.0
	ln -sf $@.1 $@


#overall functions
all: $(lib_b)

.PHONY= clean
clean:
	@ echo "cleaning libraries *.so files"
	@ rm -rf *.so* ../src/*.o
