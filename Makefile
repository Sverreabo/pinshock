EXE := new_pinshock.out
OBJS := new_pinshock.o pingame/pingame.o pingame/bitboard.o
CXX := g++
CXXFLAGS := -Wall -Werror -std=gnu++2a -O2

all: $(EXE)

run: $(EXE)
	./$(EXE)

$(EXE): $(OBJS)
	$(CXX) $(OBJS) -o $(EXE)

$(OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -f $(OBJS) $(EXE)