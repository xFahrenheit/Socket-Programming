all: serverM.cpp serverC.cpp serverEE.cpp serverCS.cpp client.cpp
	g++ -std=c++11 -o serverM serverM.cpp
	g++ -std=c++11 -o serverC serverC.cpp
	g++ -std=c++11 -o serverEE serverEE.cpp
	g++ -std=c++11 -o serverCS serverCS.cpp
	g++ -std=c++11 -o client client.cpp

PHONY: serverM
serverM: ./serverM

PHONY: serverC
serverC: ./serverC

PHONY: serverEE
serverEE: ./serverEE

PHONY: serverCS
serverCS: ./serverCS

PHONY: client
client: ./client