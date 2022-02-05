#include <iostream>
#include <fstream>
#include <string_view>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <climits>
#include <iomanip>
#include <type_traits>
#include <bit>
#include <cassert>

#include "identifiers.h"

int64_t ReverseBits(const char* p1, const char* p2)
{
	assert(CHAR_BIT == 8);
	constexpr int n = 64 / 8;

	int64_t result{ 0 };
	int cnt{ 1 };
	for (char* p = const_cast<char*>(p1); p < p2; ++p)
	{
		char raw = *p;
		char rev = 0;

		for (auto i = 0; i < 8; ++i)
		{
			rev = rev << 1 | (raw & 1);
			raw >>= 1;
		}

		if (cnt <= 8)
		{
			int64_t mask = rev;
			mask <<= (8 * (8 - (int64_t)cnt));
			result += mask;
			cnt++;
		}

	}
	return result;
}


enum class TokenType { Keyword, Number, Register, Pass};

using Token = std::pair<TokenType, int16_t>;

void Tokenize(const std::string_view& str, std::vector<Token>& tokens)
{
	for (int i = 0; i < str.size(); ++i)
	{
		auto ch = str[i];

		if (std::isspace(ch)) {
			continue;
		}

		int begin = i;

		if ((ch >= 'A' && ch <= 'Z')) 
		{
			while ((str[i] >= 'A' && str[i] <= 'Z')) {
				++i;
			}

			auto token = std::string_view(str.substr(begin, (size_t)i - (size_t)begin));
			auto index = FindInstruction(token);
			if (index != kNotIndex)
			{
				tokens.emplace_back(TokenType::Keyword, index);
			}
			else {
				index = FindRegister(token);
				if (index != kNotIndex)
				{
					tokens.emplace_back(TokenType::Register, index);
				}
			}
		}
		else
			if ((ch >= '0' && ch <= '9') || ch == '-')
			{
				char sign = 1;
				if (ch == '-')
				{
					begin = ++i;
					ch = str[i];
					sign = -1;
				}

				auto nToken = (int16_t)ch - (int16_t)'0';
				if (nToken > 0)
				{
					// NOTE: decimal
					while (str[i] >= '0' && str[i] <= '9') {
						++i;
					} 
					auto token = std::string_view(str.substr(begin, (size_t)i - (size_t)begin));
					nToken = std::atoll(token.data());
				}
				else
				{
					if (str[i + (size_t)1] == 'x' || str[i + (size_t)1] == 'X')
					{
						// NOTE: hex
						i += 2;
						begin = i;
						while ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'f') || (str[i] >= 'A' && str[i] <= 'F')) {
							++i;
						}
						auto token = std::string_view(str.substr(begin, (size_t)i - (size_t)begin));
						auto ul = std::strtoul(token.data(), nullptr, 16);
						nToken = static_cast<int16_t>(ul);
					}
					else
					{
						// NOTE: oct
						++i;
						begin = i;
						while ((str[i] >= '0' && str[i] <= '7')) {
							++i;
						}
						auto token = std::string_view(str.substr(begin, (size_t)i - (size_t)begin));
						auto ul = std::strtoul(token.data(), nullptr, 8);
						nToken = static_cast<int16_t>(ul);
					}
				}

				tokens.emplace_back(TokenType::Number, static_cast<int16_t>(nToken * sign));
			}
			else if (ch == ';')
			{
				tokens.emplace_back(TokenType::Pass, 0);
				return;
			}

	}
}


int main(int argc, char** argv)
{
	argc--;
	argv++;

	if (argc < 1) 
	{
		std::cout << "Only one argument is supported - the filename (with sources) e.g. \"my_src.asm\"" << std::endl;
		return EXIT_FAILURE;
	}

	auto filename = std::string_view(argv[0]);
	auto objfn = std::string(filename.substr(0, filename.size() - 4)) + ".obj";

	std::ifstream is(filename.data(), std::ios::in | std::ios::binary);

	if (!is) {
		std::cerr << "Some problems have acquired when opening the file. Does file exist? ";
		std::cerr << filename;
		return EXIT_FAILURE;
	};

	if (!is.is_open()) {
		std::cerr << "Bad filename for file with sources " << filename << std::endl;
		return EXIT_FAILURE;
	}

	std::string line;
	std::vector<Token> tokens;

	while (std::getline(is, line))
	{
		Tokenize(line, tokens);
	}

	std::ofstream os(objfn.data(), std::ios::out | std::ios::binary);

	for (const auto& code : tokens) {
		if constexpr (std::endian::native == std::endian::big)
		{
			auto val = ReverseBits(reinterpret_cast<const char*>(&code.second), reinterpret_cast<const char*>(&code.second) + (char)8);
			os.write(reinterpret_cast<const char*>(&val), sizeof(int64_t));
		}
		else {
			os.write(reinterpret_cast<const char*>(&code.second), sizeof(int64_t));
		}
	}

	return EXIT_SUCCESS;
}