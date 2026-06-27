#include "utils.hpp"

#include <cctype>
#include <string>

std::string base64_encode(const std::string& input) {
	static const char g_base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
										 "abcdefghijklmnopqrstuvwxyz"
										 "0123456789+/";
	std::string		  output;
	unsigned char	  a3[3];
	unsigned char	  a4[4];
	size_t			  i	  = 0;
	size_t			  len = input.size();

	output.reserve(((len + 2) / 3) * 4);

	while (len--) {
		a3[i++] = static_cast<unsigned char>(input[input.size() - len - 1]);

		if (i == 3) {
			a4[0] = (a3[0] & 0xfc) >> 2;
			a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
			a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
			a4[3] = a3[2] & 0x3f;

			for (i = 0; i < 4; ++i)
				output += g_base64_table[a4[i]];

			i = 0;
		}
	}
	if (i) {
		size_t j;

		for (j = i; j < 3; ++j)
			a3[j] = '\0';

		a4[0] = (a3[0] & 0xfc) >> 2;
		a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
		a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
		a4[3] = a3[2] & 0x3f;

		for (j = 0; j < i + 1; ++j)
			output += g_base64_table[a4[j]];

		while (i++ < 3)
			output += '=';
	}
	return output;
}

static int base64_value(unsigned char c) {
	if (c >= 'A' && c <= 'Z')
		return c - 'A';
	if (c >= 'a' && c <= 'z')
		return c - 'a' + 26;
	if (c >= '0' && c <= '9')
		return c - '0' + 52;
	if (c == '+')
		return 62;
	if (c == '/')
		return 63;
	return -1;
}

bool base64_decode(const std::string& input, std::string& output) {
	output.clear();

	unsigned char a4[4];
	unsigned char a3[3];
	size_t		  i = 0;

	for (size_t pos = 0; pos < input.size(); ++pos) {
		unsigned char c = static_cast<unsigned char>(input[pos]);

		if (std::isspace(c))
			continue;

		if (c == '=') {
			a4[i++] = 0;
		} else {
			int value = base64_value(c);

			if (value < 0)
				return false;

			a4[i++] = static_cast<unsigned char>(value);
		}

		if (i == 4) {
			a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
			a3[1] = ((a4[1] & 0x0f) << 4) + ((a4[2] & 0x3c) >> 2);
			a3[2] = ((a4[2] & 0x03) << 6) + a4[3];

			if (input[pos] == '=') {
				if (pos > 0 && input[pos - 1] == '=') {
					output += static_cast<char>(a3[0]);
				} else {
					output += static_cast<char>(a3[0]);
					output += static_cast<char>(a3[1]);
				}

				for (++pos; pos < input.size(); ++pos) {
					if (!std::isspace(static_cast<unsigned char>(input[pos])))
						return false;
				}

				return true;
			} else {
				output += static_cast<char>(a3[0]);
				output += static_cast<char>(a3[1]);
				output += static_cast<char>(a3[2]);
			}

			i = 0;
		}
	}
	return i == 0;
}