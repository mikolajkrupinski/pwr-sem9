CXX := g++
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra

COMMON_SRC := collision-check.cpp
COMMON_OBJ := $(COMMON_SRC:.cpp=.o)

all: ex2

$(COMMON_OBJ): collision-check.cpp collision-check.hpp md5.cpp
	$(CXX) $(CXXFLAGS) -c collision-check.cpp -o $(COMMON_OBJ)

ex2: ex2.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) ex2.cpp $(COMMON_OBJ) -o ex2

clean:
	rm -f $(COMMON_OBJ) ex2

.PHONY: all clean
