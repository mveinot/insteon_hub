CXX = g++
CURLLIBS = `curl-config --libs`
LIBS = $(CURLLIBS)
CXXFLAGS = -Wall -O2
TARGET = hub_command

OBJECTS = Insteon.o tinyxml2.o main.o

.PHONY: all clean

all:	main

main:	$(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $(TARGET)

%.o:	%.cpp
	$(CXX) -c $(CSSFLAGS) $(INCLUDES) $*.cpp -o $*.o

clean:
	rm -f $(OBJECTS) $(TARGET)
