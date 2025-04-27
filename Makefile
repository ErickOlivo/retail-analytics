# ---- toolchain ------------------------------------------------------------
CXX      := g++
CXXFLAGS := -std=c++20 -O2 -pthread                    \
            -Iinclude/extern/CLI11/include            \
            -Iinclude/extern/barkeep                  \
            -Iinclude
LDFLAGS  := -pthread

# ---- build targets --------------------------------------------------------
OBJ := src/main.o
BIN := retail

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
