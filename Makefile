pinshock.o: pinshock.cpp
	g++ -Wall -std=gnu++2a -O3 -s pinshock.cpp -o pinshock.o 

run: pinshock.o
	./pinshock.o

time: pinshock.o
	time ./pinshock.o

perf: pinshock.o
	sudo perf stat ./pinshock.o
