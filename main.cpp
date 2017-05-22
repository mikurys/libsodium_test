#include <iostream>
#include <sodium.h>
#include <chrono>
#include <cmath>

constexpr size_t SIZE = 32*1024*1024;
constexpr size_t ciphertext_len = crypto_secretbox_MACBYTES * SIZE + SIZE;

typedef std::chrono::steady_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::nanoseconds ns;
typedef std::chrono::duration<double> fsec;

unsigned char nonce[crypto_secretbox_NONCEBYTES];
unsigned char key[crypto_secretbox_KEYBYTES];
unsigned char *buff = new unsigned char[SIZE];
unsigned char *ciphertext = new unsigned char[ciphertext_len];

void init(){
	randombytes_buf(nonce, sizeof nonce);
	randombytes_buf(key, sizeof key);
	randombytes_buf(buff, SIZE);
}

size_t encrypt(unsigned char *ciphertext, unsigned char *buff, size_t size, size_t sizeB){
	int number_of_parts = static_cast<size_t>(std::floor(static_cast<double>(size)/sizeB));
	size_t true_size = number_of_parts * sizeB;
	unsigned char *buff_ptr = buff;
	unsigned char *ciphertext_ptr = ciphertext;
	while(buff_ptr+sizeB <= buff+size)
	{
		crypto_secretbox_easy(ciphertext_ptr, buff_ptr, sizeB, nonce, key);
		buff_ptr += sizeB;
		ciphertext_ptr += sizeB + crypto_secretbox_MACBYTES;
	}
	return true_size;
}

int main(int argc, char **argv)
{
	size_t iterations = 1;
	if (argc == 2)
		iterations = atoi(argv[1]);
	if (sodium_init() == -1) {
		return 1;
	}

	init();
	for (size_t size=10; size<=SIZE; size+=10)
	{
		if(size > 2000 && size%500 != 0) continue;
		else if(size > 9000 && size%1000 != 0) continue;
		else if(size > 100000 && size%100000 != 0) continue;
		else if(size > 1000000 && size%1000000 != 0) continue;
		else if(size > 10000000 && size%10000000 != 0) continue;
		//unsigned char *buff = new unsigned char[size];
		//randombytes_buf(buff, size);
		for (size_t sizeB=1; sizeB<=size; sizeB++)
		{
			if(sizeB > 256 && sizeB%16 != 0) continue;
			else if(sizeB > 2048 && sizeB%128 != 0) continue;
			else if(sizeB > 16*1024 && sizeB%2048 != 0) continue;
			else if(sizeB > 256*1024 && sizeB%(256*1024) != 0) continue;
			else if(sizeB > 2*1024*1024 && sizeB%(2*1024*1024) != 0) continue;
			else if(sizeB > 32*1024*1024 && sizeB%(32*1024*1024) != 0) continue;
			else if(sizeB > 512*1024*1024 && sizeB%(512*1024*1024) != 0) continue;

			size_t true_size;
			auto start_time = Time::now();
			size_t iter = iterations*(std::ceil(static_cast<double>(SIZE)/(size*5000)));
			for (size_t i=0; i<iter; i++)
			{
			  	true_size = encrypt(ciphertext, buff, size, sizeB);
			}
			auto end_time = Time::now();

			fsec time = end_time - start_time;
			//ms m = std::chrono::duration_cast<ms>(time);
			ns n = std::chrono::duration_cast<ns>(time);
			//std::cout << "Buffer size: " << size << " crypto_secretbox_easy time in ns: " << m.count()	<< std::endl;
			std::cout << size << " " << sizeB << " " << (static_cast<double>(size)*1000*1000*1000)/(static_cast<double>(n.count())*1024*1024)*8*iter	<< std::endl;

			//std::cerr << std::endl << "iter=" << iter << " size=" << size << " SIZE=" << SIZE << " n.count=" << n.count() << std::endl;
		}
		std::cout << std::endl;
		std::cerr << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << size << '/' << SIZE;
	}
	delete[] buff;
	delete[] ciphertext;
	return 0;
}
