FLAGS = -std=c++20 -O3 -Wall -I/opt/homebrew/opt/boost/include
BOOST_PATH = /opt/homebrew/opt/boost/include
INCLUDE_PATH = ./include
SOURCE_FILES = source/main.cpp source/field.cpp source/fp-element.cpp source/fpk-element.cpp source/f2k-element.cpp
OUTPUT_FILE = main.out

all:
	g++ $(FLAGS) -I$(BOOST_PATH) -I$(INCLUDE_PATH) $(SOURCE_FILES) -o $(OUTPUT_FILE)