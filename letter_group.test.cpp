//@	{"target":{"name":"letter_group.test", "actions":["run"]}}

#include "./letter_group.hpp"

#include "testfwk/testfwk.hpp"

#include <set>

TESTCASE(wordgen_letter_group_create)
{
	wordgen::letter_group foo{"aa"};
	auto bar = foo;
	EXPECT_EQ(foo, bar);

	bar = wordgen::letter_group{"bb"};
	EXPECT_NE(foo, bar);

	EXPECT_EQ(foo.value(), "aa");
}

namespace
{
	struct letter_group_cmp
	{
		using is_transparent = int;

		bool operator()(wordgen::letter_group const& a, wordgen::letter_group const& b) const
		{
			return a.value() < b.value();
		}

		bool operator()(wordgen::letter_group const& a, std::string_view b) const
		{
			return a.value() < b;
		}

		bool operator()(std::string_view a, wordgen::letter_group const& b) const
		{
			return a < b.value();
		}
	};
}

TESTCASE(wordgen_letter_group_split_word)
{
	std::set<wordgen::letter_group, letter_group_cmp> letter_groups{
		wordgen::letter_group{"k"},
		wordgen::letter_group{"a"},
		wordgen::letter_group{"ll"},
		wordgen::letter_group{"e"}
	};

	auto token = next_group(std::string_view{"kalle"}, letter_groups);
	EXPECT_EQ(token.first, "k");
	token = next_group(token.second, letter_groups);
	EXPECT_EQ(token.first, "a");
	token = next_group(token.second, letter_groups);
	EXPECT_EQ(token.first, "ll");
	token = next_group(token.second, letter_groups);
	EXPECT_EQ(token.first, "e");
	EXPECT_EQ(std::size(token.second), 0);
}

TESTCASE(wordgen_letter_group_split_word_unknown_letters)
{
	std::set<wordgen::letter_group, letter_group_cmp> letter_groups{
		wordgen::letter_group{"k"},
		wordgen::letter_group{"a"},
		wordgen::letter_group{"ll"},
		wordgen::letter_group{"e"}
	};

	auto token = next_group(std::string_view{"bad"}, letter_groups);
	EXPECT_EQ(std::size(token.first), 0);
	EXPECT_EQ(std::size(token.second), 0);
}