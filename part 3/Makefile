CXX = g++
CXXFLAGS = -std=c++17

EXES = serverA serverB servermain client

all: $(EXES)

servermain: servermain.cpp
	$(CXX) $(CXXFLAGS) -o servermain servermain.cpp

serverA: serverA.cpp
	$(CXX) $(CXXFLAGS) -o serverA serverA.cpp

serverB: serverB.cpp
	$(CXX) $(CXXFLAGS) -o serverB serverB.cpp

client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp

.PHONY: run-main-server run-serverA run-serverB run-client

run-main-server: servermain
	./servermain

run-serverA: serverA
	./serverA

run-serverB: serverB
	./serverB

run-client: client
	./client

clean:
	rm -f $(EXES)