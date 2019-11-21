#list of all src and object files
csrc := $(wildcard ../src/*.c)
cobj := $(csrc:.c=.o)

#directories containing the headers, libraries and executables
chead:= ../include

#flags necessary to the compilation
CC := gcc
CFLAGS:= -fPIC -Wall -Werror -g -I$(chead)
lib_b:= libscreen.so libnetwork.so libdataset.so libalgo.so libserialisation.so

#objects compilation from the source files
%.o: %.c
	echo "Building $@"
	$(CC) $(CFLAGS) -o $@ -c $<


#libraries compilation and linking (version number -> *.so file)
libscreen.so : ../src/screen.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.0 $<
	ldconfig -l libscreen.so.1.0
	ln -sf $@.1 $@

libnetwork.so : ../src/network.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.0 $<
	ldconfig -l libnetwork.so.1.0
	ln -sf $@.1 $@

libalgo.so : ../src/algo.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.1 $<
	ldconfig -l libalgo.so.1.1
	ln -sf $@.1 $@

libdataset.so : ../src/dataset.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.2 $<
	ldconfig -l libdataset.so.1.2
	ln -sf $@.1 $@

libserialisation.so : ../src/serialisation.o
	echo "Building $@"
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.1 $<
	ldconfig -l libserialisation.so.1.1
	ln -sf $@.1 $@


#overall functions
all: $(lib_b)

.PHONY= clean
clean:
	rm -rf *.so* ../src/*.o
