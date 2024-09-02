CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

EXES = serverA servermain serverB

all: $(EXES)

servermain: servermain.cpp
	$(CXX) $(CXXFLAGS) -o servermain servermain.cpp

serverA: serverA.cpp
	$(CXX) $(CXXFLAGS) -o serverA serverA.cpp

serverB: serverB.cpp
	$(CXX) $(CXXFLAGS) -o serverB serverB.cpp

.PHONY: run-main-server run-serverA run-serverB

run-main-server: servermain
	./servermain

run-serverA: serverA
	./serverA

run-serverB: serverB
	./serverB

clean:
	rm -f -v $(EXES)
