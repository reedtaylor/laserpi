CXX = g++
CXXFLAGS = -std=c++11
LDLIBS = -lwiringPi -lstdc++

HEADERS = laserpi.hpp
OBJECTS = laserpi.o

default: laserpi

RumPi: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJECTS): $(HEADERS)

clean:
	-rm -f $(OBJECTS)
	-rm -f laserpi

