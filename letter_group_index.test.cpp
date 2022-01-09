//@	{"target":{"name":"letter_group_index.test", "actions":["run"]}}

#include "./letter_group_index.hpp"

#include "testfwk/testfwk.hpp"

#include <queue>
#include <algorithm>

TESTCASE(wordgen_letter_group_index_id_increment)
{
	wordgen::letter_group_id id{};
	EXPECT_EQ(id.value(), 0);
	auto id_old = id;
	++id;
	EXPECT_EQ(id.value(), 1);
	EXPECT_GT(id, id_old);
	EXPECT_LT(id_old, id);
	id_old = id;
	auto id2 = id++;
	EXPECT_EQ(id2, id_old);
	EXPECT_EQ(id.value(), 2);
	auto const str = to_string(id);
	EXPECT_EQ(str, "2");
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

TESTCASE(wordgen_letter_group_index_insert_and_get)
{
	std::deque<wordgen::letter_group> letter_groups;

	letter_groups.push_back(wordgen::letter_group{"sch"});
	letter_groups.push_back(wordgen::letter_group{"s"});
	letter_groups.push_back(wordgen::letter_group{"c"});
	letter_groups.push_back(wordgen::letter_group{"h"});
	letter_groups.push_back(wordgen::letter_group{"e"});
	letter_groups.push_back(wordgen::letter_group{"l"});
	letter_groups.push_back(wordgen::letter_group{"l"});
	letter_groups.push_back(wordgen::letter_group{"a"});
	letter_groups.push_back(wordgen::letter_group{"g"});
	letter_groups.push_back(wordgen::letter_group{"r"});

	std::ranges::sort(letter_groups, letter_group_cmp{});

	auto letter_groups_in = std::queue{letter_groups};

	using storage = wordgen::letter_group_index;
	auto letter_groups_out = load(std::type_identity<storage>{}, letter_groups_in);
	static_assert(std::is_same_v<decltype(letter_groups_out), storage>);

	EXPECT_EQ(std::size(letter_groups_out) - 1, std::size(letter_groups) - 1);

	std::ranges::for_each(std::begin(letter_groups), std::end(letter_groups), [current_id = wordgen::letter_group_id{1},
		&letter_groups_out,
		prev = std::string_view{},
		expected_id = wordgen::letter_group_id{}
	](auto const& item) mutable {
		if(item.value() != prev)
		{
			expected_id = current_id;
			++current_id;
		}

		REQUIRE_EQ(letter_groups_out.contains(item), true);
		EXPECT_EQ(letter_groups_out.get(item), expected_id);
		REQUIRE_EQ(letter_groups_out.contains(expected_id), true);
		EXPECT_EQ(letter_groups_out.get(expected_id), item);
		prev = item.value();
	});
}