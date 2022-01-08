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

	auto token = next_group_longest(std::string_view{"kalle"}, letter_groups);
	EXPECT_EQ(token.first, "k");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "a");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "ll");
	token = next_group_longest(token.second, letter_groups);
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

	auto token = next_group_longest(std::string_view{"bad"}, letter_groups);
	EXPECT_EQ(std::size(token.first), 0);
	EXPECT_EQ(std::size(token.second), 0);
}

TESTCASE(wordgen_letter_group_split_word_unknown_letters_not_begin)
{
	std::set<wordgen::letter_group, letter_group_cmp> letter_groups{
		wordgen::letter_group{"k"},
		wordgen::letter_group{"a"},
		wordgen::letter_group{"ll"},
		wordgen::letter_group{"e"}
	};

	auto token = next_group_longest(std::string_view{"kolla"}, letter_groups);
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(std::size(token.first), 0);
	EXPECT_EQ(std::size(token.second), 0);
}

TESTCASE(wordgen_letter_group_split_word_match_longest)
{
	std::set<wordgen::letter_group, letter_group_cmp> letter_groups{
		wordgen::letter_group{"sch"},
		wordgen::letter_group{"s"},
		wordgen::letter_group{"c"},
		wordgen::letter_group{"h"},
		wordgen::letter_group{"e"},
		wordgen::letter_group{"j"},
		wordgen::letter_group{"l"},
		wordgen::letter_group{"a"},
		wordgen::letter_group{"g"},
		wordgen::letter_group{"r"},
	};

	auto token = next_group_longest(std::string_view{"schlager"}, letter_groups);
	EXPECT_EQ(token.first, "sch");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "l");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "a");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "g");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "e");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "r");
}

TESTCASE(wordgen_letter_group_split_word_match_shortest)
{
	std::set<wordgen::letter_group, letter_group_cmp> letter_groups{
		wordgen::letter_group{"sch"},
		wordgen::letter_group{"s"},
		wordgen::letter_group{"c"},
		wordgen::letter_group{"h"},
		wordgen::letter_group{"e"},
		wordgen::letter_group{"j"},
		wordgen::letter_group{"l"},
		wordgen::letter_group{"a"},
		wordgen::letter_group{"g"},
		wordgen::letter_group{"r"},
	};

	auto token = next_group_shortest(std::string_view{"schlager"}, letter_groups);
	EXPECT_EQ(token.first, "s");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "c");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "h");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "l");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "a");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "g");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "e");
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(token.first, "r");
	EXPECT_EQ(std::size(token.second), 0);
}

