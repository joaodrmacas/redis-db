CXX = g++
CXXFLAGS = -std=c++17 -Wall

SRC_USER = client/client.cpp utility.cpp
OBJ_USR = $(SRC_USER:.cpp=.o)
TARGET_USER = cliente

SRC_SV = server/server.cpp server/conn.cpp utility.cpp
OBJ_SV = $(SRC_SV:.cpp=.o)
TARGET_SV = servidor

.PHONY: all clean run_client run_server

all: $(TARGET_USER) $(TARGET_SV)

$(TARGET_USER): $(OBJ_USR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TARGET_SV): $(OBJ_SV)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_USR) $(OBJ_SV) $(TARGET_USER) $(TARGET_SV)

run_client: $(TARGET_USER)
	./$(TARGET_USER)

run_server: $(TARGET_SV)
	./$(TARGET_SV)
