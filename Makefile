pinshock.out: pinshock.cpp
	g++ -Wall -std=gnu++2a -O3 -s pinshock.cpp -o pinshock.out 

run: pinshock.out
	./pinshock.out

time: pinshock.out
	time ./pinshock.out

perf: pinshock.out
	sudo perf stat ./pinshock.out
