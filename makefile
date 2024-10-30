CXX = g++
CXXFLAGS = -g


files = main.cpp
objects = $(files:.cpp=.o)

target = main

all: $(target)

$(target): $(objects)
	$(CXX) $(CXXFLAGS) -o $(target) $(objects)
%.o: %.cpp $(deps)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(objects) $(target)

