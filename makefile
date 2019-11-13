# Makefile for all libraries, client and server
# Warning : when executing programs, do not forget to run 'export LD_LIBRARY_PATH=./lib/:../lib'

# Project name
Client := client
Server := server

#paths for Object files (Opath)
Opath := src
Bpath := bin

#Library & header paths
Incpath := include
Libpath := lib

#Compiler and common flags
CXX = gcc -fPIC -Wall -Werror

#Library list
Libflags := -lnetwork -lscreen -ldataset -lalgo	-lserialisation
Libbuilds := libnetwork libscreen libdataset libalgo libserialisation

#
# Actual build targets
#

$(Client) : $(Client).c $(Libbuilds)
	echo 'Building' $@
	$(CXX) -L$(Libpath) -I$(Incpath) -o$(Bpath)/$@ $@.c $(Libflags)

$(Server) : $(Server).c $(Libbuilds)
	echo 'Building' $@
	$(CXX) -L$(Libpath) -I$(Incpath) -o $(Bpath)/$@ $@.c $(Libflags)

libnetwork : $(Opath)/network.c
	echo 'Building' $@
	$(CXX) -I$(Incpath) -c $^ -o $(Opath)/network.o
	$(CXX) -shared -Wl,-soname,$@.so.2 -o $(Libpath)/$@.so.2.0 $(Opath)/network.o
	ldconfig -n $(Libpath)/
	ln -sf $@.so.2 $(Libpath)/$@.so

libscreen : $(Opath)/screen.c
	echo 'Building' $@
	$(CXX) -I$(Incpath) -c $^ -o $(Opath)/screen.o
	$(CXX) -shared -Wl,-soname,$@.so.1 -o $(Libpath)/$@.so.1.0 $(Opath)/screen.o
	ldconfig -n $(Libpath)/
	ln -sf $@.so.1 $(Libpath)/$@.so

libdataset : $(Opath)/dataset.c
	echo 'Building' $@
	$(CXX) -I$(Incpath) -c $^ -o $(Opath)/dataset.o
	$(CXX) -shared -Wl,-soname,$@.so.1 -o $(Libpath)/$@.so.1.0 $(Opath)/dataset.o
	ldconfig -n $(Libpath)/
	ln -sf $@.so.1 $(Libpath)/$@.so

libalgo : $(Opath)/algo.c
	echo 'Building' $@
	$(CXX) -I$(Incpath) -c $^ -o $(Opath)/algo.o
	$(CXX) -shared -Wl,-soname,$@.so.1 -o $(Libpath)/$@.so.1.0 $(Opath)/algo.o
	ldconfig -n $(Libpath)/
	ln -sf $@.so.1 $(Libpath)/$@.so

libserialisation : $(Opath)/serialisation.c
	echo 'Building' $@
	$(CXX) -I$(Incpath) -c $^ -o $(Opath)/serialisation.o
	$(CXX) -shared -Wl,-soname,$@.so.1 -o $(Libpath)/$@.so.1.0 $(Opath)/serialisation.o
	ldconfig -n $(Libpath)/
	ln -sf $@.so.1 $(Libpath)/$@.so

#
# install and clean targets
#

install :
	echo 'Creating directories and building everything'
	mkdir -p $(Bpath) $(Libpath)
	make $(Client) $(Server)

uninstall :
	make clean
	echo 'Deleting directories'
	rm -rf $(Bpath) $(Libpath)
clean:
	echo 'Deleting builds'
	rm -rf $(Opath)/*.o $(Libpath)/*.so* bin/*

