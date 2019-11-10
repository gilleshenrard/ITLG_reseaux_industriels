# Makefile for all libraries, client and server
# Warning : when executing programs, do not forget to run 'export LD_LIBRARY_PATH=./lib/'

# Project name
Client := client
Server := server

#paths for Object files (Opath)
Opath := src

#Library & header paths
Incpath := include
Libpath := lib

CXX = gcc

Obj := $(Opath)\network.o $(Opath)\screen.o
Libflags := -lnetwork -lscreen
Libbuilds := libnetwork libscreen

install :
	make $(Libbuilds) $(Client) $(Server)

$(Client) :
	$(CXX) -Wall -Werror -L$(Libpath) -I$(Incpath) -o $@ $@.c $(Libflags)

$(Server) :
	$(CXX) -Wall -Werror -L$(Libpath) -I$(Incpath) -o $@ $@.c $(Libflags)

libnetwork : $(Opath)/network.c
	$(CXX) -Wall -Werror -fPIC -I$(Incpath) -c $^ -o $(Opath)/network.o
	mkdir -p $(Libpath)
	$(CXX) -fPIC -shared -Wl,-soname,$@.so.1 -o $(Libpath)/$@.so.1.0 $(Opath)/network.o
	ldconfig -n $(Libpath)/
	ln -sf $@.so.1 $(Libpath)/$@.so

libscreen : $(Opath)/screen.c
	$(CXX) -Wall -Werror -fPIC -I$(Incpath) -c $^ -o $(Opath)/screen.o
	mkdir -p $(Libpath)
	$(CXX) -fPIC -shared -Wl,-soname,$@.so.1 -o $(Libpath)/$@.so.1.0 $(Opath)/screen.o
	ldconfig -n $(Libpath)/
	ln -sf $@.so.1 $(Libpath)/$@.so

clean:
	rm -rf $(Opath)/*.o $(Libpath)/*.so* $(Client) $(Server)
