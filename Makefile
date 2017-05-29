PROGRAM_OPTION=--auth #posibilities: --auth, --encrypt-auth
MIN_SIZE=64024 #minimum data to calculate avg crypto speed, default value 1024 (increase to get better data)
QUALITY=128 #chart quality, default value 4 (increase to get better chart)
all:
	g++ -std=c++14 -lsodium -o libsodium_test main.cpp
run:
	g++ -std=c++14 -lsodium -o libsodium_test main.cpp
	./libsodium_test
test:
	g++ -std=c++14 -lsodium -O3 -o libsodium_test main.cpp
	time ./libsodium_test $(MIN_SIZE) $(QUALITY) $(PROGRAM_OPTION) > result.txt
	gnuplot gplot_skrypt
