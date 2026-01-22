# Kompilator i flagi
CXX = g++
CXXFLAGS = -std=c++20 -O3 -Wall

# Ścieżki do nagłówków
BOOST_PATH = /opt/homebrew/opt/boost/include
OPENSSL_PATH = /opt/homebrew/opt/openssl@3
INCLUDE_PATH = ./include
INCLUDES = -I$(BOOST_PATH) -I$(OPENSSL_PATH)/include -I$(INCLUDE_PATH)
LINKS = -L$(OPENSSL_PATH)/lib -lssl -lcrypto

# Katalog na pliki obiektowe (.o)
OBJ_DIR = obj

# Pliki źródłowe
SRCS = source/main.cpp \
       source/common.cpp \
       source/field.cpp \
       source/fp-element.cpp \
       source/fpk-element.cpp \
       source/f2k-element.cpp \
       source/elliptic-curve.cpp \
       source/elliptic-curve-fp.cpp \
       source/elliptic-curve2.cpp \
	   source/ghash.cpp

# Lista plików obiektowych:
# Zamienia 'source/plik.cpp' na 'obj/source/plik.o'
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

# Nazwa pliku wynikowego
OUTPUT_FILE = main.out

# Domyślny cel
all: $(OUTPUT_FILE)

# Linkowanie
# Buduje plik wykonywalny z plików .o znajdujących się w folderze obj/
$(OUTPUT_FILE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LINKS) -o $(OUTPUT_FILE)

# Kompilacja
# Reguła dla plików w folderze OBJ_DIR.
# 1. @mkdir -p $(@D) -> Tworzy katalog dla pliku docelowego (np. obj/source/), jeśli nie istnieje.
# 2. Kompiluje plik źródłowy do pliku obiektowego w folderze obj.
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Czyszczenie
# Usuwa cały folder obj oraz plik wynikowy
clean:
	rm -rf $(OBJ_DIR) $(OUTPUT_FILE)