//@	{"target":{"name":"letter_group.test", "actions":["run"]}}

#include "./letter_group.hpp"

#include "testfwk/testfwk.hpp"

#include <set>
#include <queue>

TESTCASE(langmorph_letter_group_create)
{
	langmorph::letter_group foo{"aa"};
	auto bar = foo;
	EXPECT_EQ(foo, bar);

	bar = langmorph::letter_group{"bb"};
	EXPECT_NE(foo, bar);

	EXPECT_EQ(foo.value(), "aa");
}

namespace
{
	struct letter_group_cmp
	{
		using is_transparent = int;

		bool operator()(langmorph::letter_group const& a, langmorph::letter_group const& b) const
		{
			return a.value() < b.value();
		}

		bool operator()(langmorph::letter_group const& a, std::string_view b) const
		{
			return a.value() < b;
		}

		bool operator()(std::string_view a, langmorph::letter_group const& b) const
		{
			return a < b.value();
		}
	};
}

TESTCASE(langmorph_letter_group_split_word)
{
	const std::set<langmorph::letter_group, letter_group_cmp> letter_groups{
		langmorph::letter_group{"k"},
		langmorph::letter_group{"a"},
		langmorph::letter_group{"ll"},
		langmorph::letter_group{"e"}
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

TESTCASE(langmorph_letter_group_split_word_unknown_letters)
{
	std::set<langmorph::letter_group, letter_group_cmp> letter_groups{
		langmorph::letter_group{"k"},
		langmorph::letter_group{"a"},
		langmorph::letter_group{"ll"},
		langmorph::letter_group{"e"}
	};

	auto token = next_group_longest(std::string_view{"bad"}, letter_groups);
	EXPECT_EQ(std::size(token.first), 0);
	EXPECT_EQ(std::size(token.second), 0);
}

TESTCASE(langmorph_letter_group_split_word_unknown_letters_not_begin)
{
	const std::set<langmorph::letter_group, letter_group_cmp> letter_groups{
		langmorph::letter_group{"k"},
		langmorph::letter_group{"a"},
		langmorph::letter_group{"ll"},
		langmorph::letter_group{"e"}
	};

	auto const sv = std::string_view{"kolla"};
	auto token = next_group_longest(sv, letter_groups);
	token = next_group_longest(token.second, letter_groups);
	EXPECT_EQ(std::size(token.first), 0);
	EXPECT_EQ(std::size(token.second), 0);
	EXPECT_NE(std::end(token.second), std::end(sv));

	auto groups = split_longest(sv, letter_groups);
	EXPECT_EQ(std::size(groups), 0);
}

TESTCASE(langmorph_letter_group_split_word_match_longest)
{
	const std::set<langmorph::letter_group, letter_group_cmp> letter_groups{
		langmorph::letter_group{"sch"},
		langmorph::letter_group{"s"},
		langmorph::letter_group{"c"},
		langmorph::letter_group{"h"},
		langmorph::letter_group{"e"},
		langmorph::letter_group{"j"},
		langmorph::letter_group{"l"},
		langmorph::letter_group{"a"},
		langmorph::letter_group{"g"},
		langmorph::letter_group{"r"},
	};

	auto const sv = std::string_view{"schlager"};
	auto token = next_group_longest(sv, letter_groups);
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
	EXPECT_EQ(std::end(token.second), std::end(sv));

	auto groups = split_longest(sv, letter_groups);
	REQUIRE_EQ(std::size(groups), 6);
	EXPECT_EQ(groups[0], "sch");
	EXPECT_EQ(groups[1], "l");
	EXPECT_EQ(groups[2], "a");
	EXPECT_EQ(groups[3], "g");
	EXPECT_EQ(groups[4], "e");
	EXPECT_EQ(groups[5], "r");
}

TESTCASE(langmorph_letter_group_split_word_match_shortest)
{
	const std::set<langmorph::letter_group, letter_group_cmp> letter_groups{
		langmorph::letter_group{"sch"},
		langmorph::letter_group{"s"},
		langmorph::letter_group{"c"},
		langmorph::letter_group{"h"},
		langmorph::letter_group{"e"},
		langmorph::letter_group{"j"},
		langmorph::letter_group{"l"},
		langmorph::letter_group{"a"},
		langmorph::letter_group{"g"},
		langmorph::letter_group{"r"},
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

TESTCASE(langmorph_letter_group_load)
{
	std::queue<langmorph::letter_group> letter_groups_in;

	letter_groups_in.push(langmorph::letter_group{"sch"});
	letter_groups_in.push(langmorph::letter_group{"s"});
	letter_groups_in.push(langmorph::letter_group{"c"});
	letter_groups_in.push(langmorph::letter_group{"h"});
	letter_groups_in.push(langmorph::letter_group{"e"});
	letter_groups_in.push(langmorph::letter_group{"j"});
	letter_groups_in.push(langmorph::letter_group{"l"});
	letter_groups_in.push(langmorph::letter_group{"a"});
	letter_groups_in.push(langmorph::letter_group{"g"});
	letter_groups_in.push(langmorph::letter_group{"r"});

	using storage = std::set<langmorph::letter_group, letter_group_cmp>;

	auto letter_groups_out = load(std::type_identity<storage>{}, letter_groups_in);
	static_assert(std::is_same_v<storage, decltype(letter_groups_out)>);

	EXPECT_EQ(std::size(letter_groups_in), 0);

	const std::set<langmorph::letter_group, letter_group_cmp> letter_groups_expected{
		langmorph::letter_group{"sch"},
		langmorph::letter_group{"s"},
		langmorph::letter_group{"c"},
		langmorph::letter_group{"h"},
		langmorph::letter_group{"e"},
		langmorph::letter_group{"j"},
		langmorph::letter_group{"l"},
		langmorph::letter_group{"a"},
		langmorph::letter_group{"g"},
		langmorph::letter_group{"r"},
	};

	EXPECT_EQ(std::ranges::equal(letter_groups_out, letter_groups_expected), true);
}

