ITERATIONS=1
all:
	g++ -std=c++14 -lsodium -o libsodium_test main.cpp
run:
	g++ -std=c++14 -lsodium -o libsodium_test main.cpp
	./libsodium_test
test:
	g++ -std=c++14 -lsodium -O3 -o libsodium_test main.cpp
	time ./libsodium_test $(ITERATIONS) > result.txt
	gnuplot gplot_skrypt
