CXX = g++
CXXFLAGS = -g -Werror -Wpedantic -Wall -Iincludes

objects = $(files:.cpp=.o)
files = src/main.cpp src/bmp_reader.cpp
deps = includes/bmp_reader.hpp

target = bmpreader

all: $(target)

$(target): $(objects)
	$(CXX) $(CXXFLAGS) $(objects) -o $(target)

%.o: %.cpp $(deps)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(objects) $(target) 

