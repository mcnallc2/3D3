CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -pthread -std=c++11 $(CXXOPTIMIZE)
USERID=EDIT_MAKE_FILE
CLASSES= router_implementation.cpp

all: my-router data_injection

my-router: $(CLASSES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cpp

data_injection: $()
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cpp

clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM my-router *.tar.gz
	rm -rf *.o *~ *.gch *.swp *.dSYM data_injection *.tar.gz

clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM data_injection *.tar.gz

tarball: clean
	tar -cvf $(kellehd3).tar.gz *
