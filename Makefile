# Compilador
CXX := g++


# Flags de compilação
CXXFLAGS := -Wall -Wextra -pedantic -std=c++17 -I/usr/include/postgresql
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lncurses -lpq -lpqxx -lboost_system -lboost_filesystem

# Nome do executável
TARGET := build/fascinio

# Arquivos fonte
SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.cpp)


# Arquivos objeto
OBJ_DIR := build/obj
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Regra padrão
all: $(TARGET)

# Linkagem dos objetos
$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compilação dos objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpar arquivos gerados
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
