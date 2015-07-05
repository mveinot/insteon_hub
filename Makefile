CXX = g++
LIBS =
CXXFLAGS = -Wall -O2
TARGET = hub_command

OBJECTS = Insteon.o Device.o tinyxml2.o main.o

.PHONY: all clean

all:	main

main:	$(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $(TARGET)

%.o:	%.cpp
	$(CXX) -c $(CSSFLAGS) $(INCLUDES) $*.cpp -o $*.o

clean:
	rm -f $(OBJECTS) $(TARGET)

install:
	cp -f $(TARGET) /usr/local/bin

uninstall:
	rm -f /usr/local/bin/$(TARGET)
