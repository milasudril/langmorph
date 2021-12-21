//@	{"target":{"name":"wordgen.o"}}

#include <random>
#include <string>
#include <map>
#include <set>
#include <cstdio>
#include <optional>
#include <cuchar>
#include <climits>
#include <algorithm>

using Letter = char32_t;

namespace hoehrmann_utf8
{
	/* Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
	See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

	Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	*/

	enum class result:uint32_t{accept, reject};


	constexpr uint8_t utf8d[] =
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
		8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
		0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
		0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
		0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
		1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
		1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
		1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
	};

	auto inline	decode(result* state, char32_t* codep, uint8_t byte)
	{
		uint32_t type = utf8d[byte];

		*codep = (*state != result::accept) ?
			(byte & 0x3fu) | (*codep << 6) :
			(0xff >> type) & (byte);

		*state = static_cast<result>(utf8d[256 + static_cast<uint32_t>(*state) * 16 + type]);
		return *state;
	}
}

auto single_vowels()
{
	return std::set<Letter>{
		U'a',
		U'e',
		U'i',
		U'o',
		U'u',
		U'y',
		U'ä',
		U'å',
		U'é',
		U'ö',
		U'ü'};
}

auto single_consonants()
{
	return std::set<Letter>{
		U'b',
		U'c',
		U'd',
		U'f',
		U'g',
		U'h',
		U'j',
		U'k',
		U'l',
		U'm',
		U'n',
		U'p',
		U'q',
		U'r',
		U's',
		U't',
		U'v',
		U'w',
		U'x',
		U'z',
		U'þ',
		U'ð'};
}

std::optional<char32_t> read_codepoint(FILE* src)
{
	Letter ret{};
	hoehrmann_utf8::result status{};
	while(true)
	{
		auto ch_in = getc(src);
		if(ch_in == EOF)
		{
			return std::optional<char32_t>{};
		}

		if(hoehrmann_utf8::decode(&status, &ret, ch_in) == hoehrmann_utf8::result::accept)
		{
			return ret;
		}
	}
}

std::string from_codepoints(std::basic_string_view<char32_t> str)
{
	std::array<char, MB_LEN_MAX> buffer{};
	std::mbstate_t state{};
	std::string ret;
	auto ptr = std::begin(str);
	while(ptr != std::end(str))
	{
		if(auto res = std::c32rtomb(std::data(buffer), *ptr, &state); res != static_cast<size_t>(-1))
		{
			auto copy_range = std::string_view{std::data(buffer), res};
			std::copy(std::begin(copy_range), std::end(copy_range), std::back_inserter(ret));
		}
		++ptr;
	}

	return ret;
}

enum class letter_class:int{vowel, consonant, junk};

auto classify(std::set<Letter> const& vowels, std::set<Letter> const& consonants, Letter letter)
{
	if(vowels.find(letter) != std::end(vowels))
	{
		return letter_class::vowel;
	}

	if(consonants.find(letter) != std::end(consonants))
	{
		return letter_class::consonant;
	}

	return letter_class::junk;
}

struct word
{
	std::vector<std::string> letter_groups;
};

auto load_words(std::set<Letter> const& vowels, std::set<Letter> const& consonants, FILE* src = stdin)
{
	std::vector<word> ret;

	std::basic_string<Letter> current_group;

	auto prev_class = letter_class::junk;
	word current_word;

	while(true)
	{
		if(auto cp = read_codepoint(src); cp.has_value())
		{
			auto letter_class = classify(vowels, consonants, *cp);

			if(letter_class == prev_class)
			{
				current_group += *cp;
			}
			else
			{
				switch(prev_class)
				{
					case letter_class::vowel:
						current_word.letter_groups.push_back(from_codepoints(current_group));
						break;

					case letter_class::consonant:
						current_word.letter_groups.push_back(from_codepoints(current_group));
						break;

					case letter_class::junk:
						ret.push_back(std::move(current_word));
						break;
				}
				current_group = *cp;
			}

			prev_class = letter_class;
		}
		else
		{
			return ret;
		}
	}
}

int main()
{
	std::setlocale(LC_ALL, "en_US.utf8");
	auto const vowels = single_vowels();
	auto const consonants = single_consonants();

	auto res = load_words(vowels, consonants);

	std::for_each(std::begin(res), std::end(res), [](auto const& item) {
		std::for_each(std::begin(item.letter_groups), std::end(item.letter_groups), [](auto const& item) {
			printf("%s-",item.c_str());
		});
		printf("\n");
	});

	return 0;
}