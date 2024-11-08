/* Nikita Kotegov st128906@student.spbu.ru Group B81 Lab_work1 */
CXX = g++
CXXFLAGS = -g

objects = $(files:.cpp=.o)
files = main.cpp bmp_reader.cpp
deps = bmp_reader.hpp

target = bmpreader

all: $(target)

$(target): $(objects)
	$(CXX) $(CXXFLAGS) $(objects) -o $(target)

%.o: %.cpp $(deps)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(objects) $(target) 
