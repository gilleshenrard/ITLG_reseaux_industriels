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

CXX = gcc

Obj := $(Opath)/network.o $(Opath)/screen.o $(Opath)/dataset.o $(Opath)/algo.o $(Opath)/serialisation.o
Libflags := -lnetwork -lscreen -ldataset -lalgo	-lserialisation
Libbuilds := libnetwork libscreen libdataset libalgo libserialisation

install :
	make $(Libbuilds) $(Client) $(Server)

$(Client) :
	mkdir -p $(Bpath)
	$(CXX) -Wall -Werror -L$(Libpath) -I$(Incpath) -o$(Bpath)/$@ $@.c $(Libflags)

$(Server) :
	mkdir -p $(Bpath)
	$(CXX) -Wall -Werror -L$(Libpath) -I$(Incpath) -o $(Bpath)/$@ $@.c $(Libflags)

libnetwork : $(Opath)/network.c
	$(CXX) -Wall -Werror -fPIC -I$(Incpath) -c $^ -o $(Opath)/network.o
	mkdir -p $(Libpath)
	$(CXX) -fPIC -shared -Wl,-soname,$@.so.2 -o $(Libpath)/$@.so.2.0 $(Opath)/network.o
	ldconfig -n $(Libpath)/
	ln -sf $@.so.2 $(Libpath)/$@.so

libscreen : $(Opath)/screen.c
	$(CXX) -Wall -Werror -fPIC -I$(Incpath) -c $^ -o $(Opath)/screen.o
	mkdir -p $(Libpath)
	$(CXX) -fPIC -shared -Wl,-soname,$@.so.1 -o $(Libpath)/$@.so.1.0 $(Opath)/screen.o
	ldconfig -n $(Libpath)/
	ln -sf $@.so.1 $(Libpath)/$@.so

libdataset : $(Opath)/dataset.c
	$(CXX) -Wall -Werror -fPIC -I$(Incpath) -c $^ -o $(Opath)/dataset.o
	mkdir -p $(Libpath)
	$(CXX) -fPIC -shared -Wl,-soname,$@.so.1 -o $(Libpath)/$@.so.1.0 $(Opath)/dataset.o
	ldconfig -n $(Libpath)/
	ln -sf $@.so.1 $(Libpath)/$@.so

libalgo : $(Opath)/algo.c
	$(CXX) -Wall -Werror -fPIC -I$(Incpath) -c $^ -o $(Opath)/algo.o
	mkdir -p $(Libpath)
	$(CXX) -fPIC -shared -Wl,-soname,$@.so.1 -o $(Libpath)/$@.so.1.0 $(Opath)/algo.o
	ldconfig -n $(Libpath)/
	ln -sf $@.so.1 $(Libpath)/$@.so

libserialisation : $(Opath)/serialisation.c
	$(CXX) -Wall -Werror -fPIC -I$(Incpath) -c $^ -o $(Opath)/serialisation.o
	mkdir -p $(Libpath)
	$(CXX) -fPIC -shared -Wl,-soname,$@.so.1 -o $(Libpath)/$@.so.1.0 $(Opath)/serialisation.o
	ldconfig -n $(Libpath)/
	ln -sf $@.so.1 $(Libpath)/$@.so

clean:
	rm -rf $(Opath)/*.o $(Libpath)/*.so* bin/*
