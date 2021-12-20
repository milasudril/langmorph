//@	{"target":{"name":"wordgen.o"}}

#include <random>
#include <string>
#include <map>
#include <set>
#include <cstdio>
#include <optional>

using Letter = char32_t;

namespace hoehrmann_utf8
{
	// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
	// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

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

auto collect_groups(std::set<Letter> const& vowels, std::set<Letter> const& consonants, FILE* src = stdin)
{
	std::map<std::basic_string<Letter>, size_t> vowel_groups;
	std::map<std::basic_string<Letter>, size_t> consonant_groups;

	enum class state_value:int{init, vowel, consonant, junk};

	auto current_state = state_value::init;

	std::basic_string<Letter> current_group;

	while(true)
	{
		if(auto cp = read_codepoint(src); cp.has_value())
		{
			switch(current_state)
			{
				case state_value::init:
					switch(auto ch_class = classify(vowels, consonants, *cp); ch_class)
					{
						case letter_class::vowel:
							current_group += *cp;
							break;
						case letter_class::consonant:
							current_group += *cp;
							break;

						case letter_class::junk:
							break;
					}
					break;
			}
		}
		else
		{
			return std::pair{std::move(vowel_groups), std::move(consonant_groups)};
		}
	}
}

int main()
{
	auto const vowels = single_vowels();
	auto const consonants = single_consonants();

	collect_groups(vowels, consonants);
}