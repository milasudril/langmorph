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
#include <memory>
#include <span>
#include <unistd.h>

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
		U'a',U'A',
		U'e',U'E',
		U'i',U'I',
		U'o',U'O',
		U'u',U'U',
		U'y',U'Y',
		U'ä',U'Ä',
		U'å',U'Å',
		U'é',U'É',
		U'ö',U'Ö',
		U'ü',U'Ü'
	};
}

auto single_consonants()
{
	return std::set<Letter>{
		U'b',U'B',
		U'c',U'C',
		U'd',U'D',
		U'f',U'F',
		U'g',U'G',
		U'h',U'H',
		U'j',U'J',
		U'k',U'K',
		U'l',U'L',
		U'm',U'M',
		U'n',U'N',
		U'p',U'P',
		U'q',U'Q',
		U'r',U'R',
		U's',U'S',
		U't',U'T',
		U'v',U'V',
		U'w',U'W',
		U'x',U'X',
		U'z',U'Z',
		U'þ',U'Þ',
		U'ð',U'Ð'
	};
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

class letter_group_table
{
public:
	using letter_group_id = size_t;

	letter_group_table():m_current_id{0}{}

	letter_group_id insert(std::string_view letter_group)
	{
		auto const i = m_group_to_id.find(letter_group);
		if(i != std::end(m_group_to_id))
		{ return i->second; }

		auto const ret = m_current_id;
		m_group_to_id.insert(i, std::pair{letter_group, ret});
		m_id_to_group.insert(std::pair{ret, letter_group});

		++m_current_id;
		return ret;
	}

	auto get(std::string_view letter_group) const
	{
		auto i = m_group_to_id.find(letter_group);
		if(i == std::end(m_group_to_id)) { return static_cast<letter_group_id>(0); }
		return i->second;
	}

	auto get(letter_group_id id) const
	{
		auto i = m_id_to_group.find(id);
		if(i == std::end(m_id_to_group)) { return std::string_view{""}; }
		return std::string_view{i->second};
	}

	size_t size() const{ return std::size(m_id_to_group); }

private:
	std::map<letter_group_id, std::string> m_id_to_group;
	std::map<std::string, letter_group_id, std::less<>> m_group_to_id;
	letter_group_id m_current_id;
};

class transition_matrix
{
public:
	explicit transition_matrix(size_t n):m_rates{std::make_unique<double[]>(n*n)}, m_size{n}{}

	std::span<double const> row(size_t n) const
	{
		return std::span{m_rates.get() + m_size*n, m_size};
	}

	std::span<double> row(size_t n)
	{
		return std::span{m_rates.get() + m_size*n, m_size};
	}

	double& operator()(size_t row, size_t col)
	{
		return *(m_rates.get() + m_size*row + col);
	}

	double operator()(size_t row, size_t col) const
	{
		return *(m_rates.get() + m_size*row + col);
	}

	size_t row_count() const { return m_size; }

private:
	std::unique_ptr<double[]> m_rates;
	size_t m_size;
};

int main()
{
	std::setlocale(LC_ALL, "en_US.utf8");
	auto const vowels = single_vowels();
	auto const consonants = single_consonants();

	auto const res = load_words(vowels, consonants);

	letter_group_table letter_groups;
	letter_groups.insert("");

	std::ranges::for_each(res, [&letter_groups] (auto const& item) {
		std::for_each(std::begin(item.letter_groups), std::end(item.letter_groups), [&letter_groups](auto const& item) {
			letter_groups.insert(item);
		});
	});

	std::set<std::string> real_words;
	std::ranges::for_each(res, [&real_words] (auto const& item) {
		std::string word;
		std::for_each(std::begin(item.letter_groups), std::end(item.letter_groups), [&word](auto const& item) {
			word.append(item);
		});
		real_words.insert(word);
	});


	transition_matrix transition_rates{std::size(letter_groups)};

	std::ranges::for_each(res, [&groups = std::as_const(letter_groups), &transition_rates] (auto const& item) {
		std::string_view prev{""};
		auto row = groups.get(prev);
		auto i = std::begin(item.letter_groups);
		while(i != std::end(item.letter_groups))
		{
			auto col = groups.get(*i);
			++transition_rates(row, col);
			row = col;
			++i;
		}
	});

	std::vector<std::discrete_distribution<size_t>> distributions;
	distributions.reserve(transition_rates.row_count());
	for(size_t k = 0; k != transition_rates.row_count(); ++k)
	{
		auto const row = transition_rates.row(k);
		distributions.push_back(std::discrete_distribution<size_t>{std::begin(row), std::end(row)});
	}

	std::mt19937 rng;
	for(size_t k = 0; k != 256; ++k)
	{
		size_t row = 0;
		size_t n = 0;
		std::string word;
		do
		{
			row = distributions[row](rng);
			word.append(letter_groups.get(row));
			++n;
		}
		while(row != 0 && n != 4);
		if(!real_words.contains(word))
		{ printf("%s\n", word.c_str()); }
	}

	return 0;
}