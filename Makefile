EXE := pinshock.out
PROFILE := profile.out
OBJS := pinshock.o pingame/pingame.o pingame/bitboard.o bitset/bitset.o
CXX := g++
CXXFLAGS := -Wall -Werror -std=gnu++2a -O2

all: $(EXE)

profile: CXXFLAGS = -Wall -Werror -std=gnu++2a -O2 -pg
profile: $(PROFILE)
	time ./$(PROFILE) -q
	gprof $(PROFILE) > profile.txt
	gprof $(PROFILE) gmon.out | gprof2dot -s -w | dot -Tpng -o output.png && code output.png

run: $(EXE)
	./$(EXE)

benchmark: clean $(EXE)
	python3 scripts/benchmark.py

$(EXE): $(OBJS)
	$(CXX) $(OBJS) -o $(EXE)

$(PROFILE): $(OBJS)
	$(CXX) $(OBJS) -o $(PROFILE) -pg


$(OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -f $(OBJS) $(EXE) $(PROFILE) gmon.out
