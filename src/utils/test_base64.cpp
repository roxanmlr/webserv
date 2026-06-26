#include "utils.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i) {
		std::string original(argv[i]);
		std::string encode = base64_encode(original);
		std::cout << argv[i] << "    =    " << encode << "\n";
		std::string decode;
		if (!base64_decode(encode, decode)) {
			std::cerr << "Error\n";
			exit(EXIT_FAILURE);
		}
		if (decode != original) {
			std::cerr << "Error 2\n";
			std::cerr << "original: " << original << "\n";
			std::cerr << "decoded : " << decode << "\n";
			exit(EXIT_FAILURE);
		}
		decode.clear();
	}
}