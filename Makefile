PROGRAM_OPTION=--encrypt #posibilities: --auth, --encrypt-auth, --encrypt
MIN_SIZE=655350 #minimum data to calculate avg crypto speed, default value 1024 (increase to get better data)
QUALITY=64 #chart quality, default value 4 (increase to get better chart)
all:
	g++ -std=c++14 -lsodium -o libsodium_test main.cpp
run:
	g++ -std=c++14 -lsodium -o libsodium_test main.cpp
	./libsodium_test
test:
	g++ -std=c++14 -lsodium -O3 -march=native -o libsodium_test main.cpp
	time ./libsodium_test $(MIN_SIZE) $(QUALITY) $(PROGRAM_OPTION) > result.txt
	gnuplot gplot_skrypt
