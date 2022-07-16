CXX = g++
CC = gcc
CXXFLAGS =
OBJECT = -c
LIBS = -lfltk
RES = -o

Client.o: Client.cpp Client.hpp
	$(CXX) $(CXXFLAGS) $< $(OBJECT) 

cJSON.o: cJSON.c cJSON.h
	$(CC) $(CXXFLAGS) $< $(OBJECT)

WeatherApp: main.cpp Client.o cJSON.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) $(RES) WeatherApp