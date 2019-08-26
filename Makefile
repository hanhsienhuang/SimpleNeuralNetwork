all:
	g++ -std=c++11 -Wall -O3 -o run main.cpp data/data_retrieval.cpp simple_network/simple_network.cpp 
