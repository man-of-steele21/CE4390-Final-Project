# Exercise4.3-4.4 Makefile
# Jackson Steele

# Files
SRC := $(wildcard *.cpp)
OBJ := $(patsubst %.cpp,%.o,$(SRC))

# Options
CC := g++
CFLAGS := -O3 -std=c++11
LDFLAGS := -L/usr/lib
LDLIBS := -lm
RM := @rm
INCLUDE :=

# Rules
host: host.o
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^
router: router.o
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^
bridge: bridge.o
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^
all: host router bridge
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $<
clean:
	$(RM) $(OBJ) $(EXEC)

# Debugging:
test:
	@echo $(SRC)
	@echo $(OBJ)
	@echo $(INCLUDE)

