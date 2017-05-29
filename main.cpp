#include <iostream>
#include <sodium.h>
#include <chrono>
#include <cmath>

//constexpr size_t SIZE = 32*1024*1024;
constexpr size_t SIZE = 2*1024;
constexpr size_t MAX_ALLIGMENT = 256;
constexpr size_t ciphertext_len = crypto_secretbox_MACBYTES * SIZE + SIZE;

typedef std::chrono::steady_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::nanoseconds ns;
typedef std::chrono::duration<double> fsec;

std::string program_option("--encrypt-auth");
std::string encrytp_auth("--encrypt-auth");
std::string auth("--auth");
std::string encryption("--encrypt");

unsigned char nonce[crypto_secretbox_NONCEBYTES];
unsigned char key[crypto_secretbox_KEYBYTES];
unsigned char *buff = new unsigned char[SIZE];
unsigned char *copy_buff = new unsigned char[SIZE+MAX_ALLIGMENT];
unsigned char *ciphertext = new unsigned char[ciphertext_len];

bool is_buffer_copied = false;

void init(){
	randombytes_buf(nonce, sizeof nonce);
	randombytes_buf(key, sizeof key);
	randombytes_buf(buff, SIZE);
}

size_t encrypt(unsigned char *ciphertext, unsigned char *buff, size_t size, size_t sizeB, size_t alligment){
	int number_of_parts = static_cast<size_t>(std::floor(static_cast<double>(size)/sizeB));
	size_t true_size = number_of_parts * sizeB;
	unsigned char *buff_ptr = buff;
	unsigned char *ciphertext_ptr = ciphertext;
	while(buff_ptr+sizeB <= buff+size)
	{
		//if(is_buffer_copied)
		std::copy(buff_ptr, buff_ptr+sizeB, copy_buff+alligment);
		if(program_option == encrytp_auth)
			crypto_secretbox_easy(ciphertext_ptr, copy_buff+alligment, sizeB, nonce, key);
		else if(program_option == auth)
			crypto_onetimeauth(ciphertext_ptr, copy_buff+alligment, sizeB, key);
		else if(program_option == encryption)
			crypto_stream_xor(ciphertext_ptr, copy_buff+alligment, sizeB, nonce, key);
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
		//if(std::string(argv[4])==std::string("--copy")) is_buffer_copied=true;
	}

	if(program_option == encrytp_auth)
		std::cout << "Encrypt+Authorize, Mega bit/sec, MIN_SIZE=" << min_size << ", QUALITY=" << quality << std::endl;
	else if(program_option == auth)
		std::cout << "Authorize, Mega bit/sec, MIN_SIZE=" << min_size << ", QUALITY=" << quality << std::endl;
	else if(program_option == encryption)
		std::cout << "Encryption, Mega bit/sec, MIN_SIZE=" << min_size << ", QUALITY=" << quality << std::endl;

	if (sodium_init() == -1) {
		return 1;
	}

	init();
	for (size_t alligment=0; alligment<=MAX_ALLIGMENT; alligment++)
	//size_t size=16;
	//double dbl_size=static_cast<double>(size), multipler = std::pow(2, 1.0/quality);
	//for ( ; size<=SIZE; size=std::round(dbl_size))
	{
		size_t size=16;
		double dbl_size=static_cast<double>(size), multipler = std::pow(2, 1.0/quality);
		for ( ; size<=SIZE; size=std::round(dbl_size))
		//for (size_t alligment=0; alligment<=MAX_ALLIGMENT; alligment++)
		{
			size_t true_size;
			auto start_time = Time::now();
			size_t iter = std::ceil(static_cast<double>(min_size)/(size));
			for (size_t i=0; i<iter; i++)
			{
			  	true_size = encrypt(ciphertext, buff, size, size, alligment);
			}
			auto end_time = Time::now();

			fsec time = end_time - start_time;
			ns n = std::chrono::duration_cast<ns>(time);
			std::cout << size << " " << alligment << " " << (static_cast<double>(true_size)*1000*1000*1000)/(static_cast<double>(n.count())*1024*1024)*8*iter	<< std::endl;
			dbl_size*=multipler;
		}
		std::cout << std::endl;
		//std::cerr << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << size << '/' << SIZE;
		//dbl_size*=multipler;
		std::cerr << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << alligment << '/' << MAX_ALLIGMENT;
	}
	delete[] buff;
	delete[] copy_buff;
	delete[] ciphertext;
	return 0;
}
