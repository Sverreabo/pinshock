pinshock.o: pinshock.cpp
	g++ -Wall -std=gnu++20 -O3 -s pinshock.cpp -o pinshock.o 

run: pinshock.o
	./pinshock.o