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

std::string program_option("--encrypt-auth");
std::string encrytp_auth("--encrypt-auth");
std::string auth("--auth");

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
		if(program_option == encrytp_auth)
			crypto_secretbox_easy(ciphertext_ptr, buff_ptr, sizeB, nonce, key);
		else if(program_option == auth)
			crypto_onetimeauth(ciphertext+sizeB, buff_ptr, sizeB, key);
		buff_ptr += sizeB;
		ciphertext_ptr += sizeB + crypto_secretbox_MACBYTES;
	}
	return true_size;
}

int main(int argc, char **argv)
{
	size_t min_size = 1024;
	size_t quality = 4;
	if (argc == 4)
	{
		min_size = atoi(argv[1]);
		quality = atoi(argv[2]);
		program_option = std::string(argv[3]);
	}

	if(program_option == encrytp_auth)
		std::cout << "Encrypt+Authorize, without allocations, Mega bit/sec, MIN_SIZE=" << min_size << ", QUALITY=" << quality << std::endl;
	else if(program_option == auth)
		std::cout << "Authorize, without allocations, Mega bit/sec, MIN_SIZE=" << min_size << ", QUALITY=" << quality << std::endl;

	if (sodium_init() == -1) {
		return 1;
	}

	init();
	size_t size=16;
	double dbl_size=static_cast<double>(size), multipler = std::pow(2, 1.0/quality);
	for ( ; size<=SIZE; size=std::round(dbl_size))
	{
	//	if(size > 2000 && size%500 != 0) continue;
	//	else if(size > 9000 && size%1000 != 0) continue;
	//	else if(size > 100000 && size%100000 != 0) continue;
	//	else if(size > 1000000 && size%1000000 != 0) continue;
	//	else if(size > 10000000 && size%10000000 != 0) continue;
		//unsigned char *buff = new unsigned char[size];
		//randombytes_buf(buff, size);
		size_t sizeB=8;
		double dbl_sizeB=static_cast<double>(sizeB);
		for (; sizeB<=size; sizeB=std::round(dbl_sizeB))
		{
		//	if(sizeB > 256 && sizeB%16 != 0) continue;
		//	else if(sizeB > 2048 && sizeB%128 != 0) continue;
		//	else if(sizeB > 16*1024 && sizeB%2048 != 0) continue;
		//	else if(sizeB > 256*1024 && sizeB%(256*1024) != 0) continue;
		//	else if(sizeB > 2*1024*1024 && sizeB%(2*1024*1024) != 0) continue;
		//	else if(sizeB > 32*1024*1024 && sizeB%(32*1024*1024) != 0) continue;
		//	else if(sizeB > 512*1024*1024 && sizeB%(512*1024*1024) != 0) continue;

			size_t true_size;
			auto start_time = Time::now();
			size_t iter = std::ceil(static_cast<double>(min_size)/(size));
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
			dbl_sizeB*=multipler;
		}
		std::cout << std::endl;
		std::cerr << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << size << '/' << SIZE;
		dbl_size*=multipler;
	}
	delete[] buff;
	delete[] ciphertext;
	return 0;
}
