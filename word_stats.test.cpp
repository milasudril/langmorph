//@	{"target":{"name":"word_stats.test", "actions":["run"]}}

#include "./word_stats.hpp"

#include "testfwk/testfwk.hpp"

#include <queue>

TESTCASE(wordgen_wordstats_load)
{
	std::queue<wordgen::letter_group> letter_groups_in;
	letter_groups_in.push(wordgen::letter_group{"sch"});
	letter_groups_in.push(wordgen::letter_group{"s"});
	letter_groups_in.push(wordgen::letter_group{"c"});
	letter_groups_in.push(wordgen::letter_group{"h"});
	letter_groups_in.push(wordgen::letter_group{"e"});
	letter_groups_in.push(wordgen::letter_group{"j"});
	letter_groups_in.push(wordgen::letter_group{"l"});
	letter_groups_in.push(wordgen::letter_group{"a"});
	letter_groups_in.push(wordgen::letter_group{"g"});
	letter_groups_in.push(wordgen::letter_group{"r"});

	std::queue<std::string> words;
	words.push(std::string{"schlager"});
	words.push(std::string{"hej"});
	words.push(std::string{"är"});

	auto letter_groups = load(std::type_identity<wordgen::letter_group_index>{}, letter_groups_in);

	wordgen::word_stats stats{words, letter_groups};

	{
		auto const& length_hist = stats.length_histogram();
		auto const largest_index = length_hist.largest_index();
		EXPECT_EQ(largest_index, wordgen::histogram_index{6});
		EXPECT_EQ(length_hist(wordgen::histogram_index{6}), 1);
		EXPECT_EQ(length_hist(wordgen::histogram_index{5}), 0);
		EXPECT_EQ(length_hist(wordgen::histogram_index{4}), 0);
		EXPECT_EQ(length_hist(wordgen::histogram_index{3}), 1);
		EXPECT_EQ(length_hist(wordgen::histogram_index{2}), 0);
		EXPECT_EQ(length_hist(wordgen::histogram_index{1}), 0);
		EXPECT_EQ(length_hist(wordgen::histogram_index{0}), 0);
	}

	{
		auto const& trans_rates = stats.transition_rates();
		EXPECT_EQ(trans_rates.node_count(), 11);
		EXPECT_EQ(trans_rates(wordgen::from_id{0},wordgen::to_id{0}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{0},wordgen::to_id{1}), 1);
		EXPECT_EQ(trans_rates(wordgen::from_id{0},wordgen::to_id{2}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{0},wordgen::to_id{3}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{0},wordgen::to_id{4}), 1);
		EXPECT_EQ(trans_rates(wordgen::from_id{0},wordgen::to_id{5}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{0},wordgen::to_id{6}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{0},wordgen::to_id{7}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{0},wordgen::to_id{8}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{0},wordgen::to_id{9}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{0},wordgen::to_id{10}), 0);

		EXPECT_EQ(trans_rates(wordgen::from_id{1},wordgen::to_id{0}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{1},wordgen::to_id{1}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{1},wordgen::to_id{2}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{1},wordgen::to_id{3}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{1},wordgen::to_id{4}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{1},wordgen::to_id{5}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{1},wordgen::to_id{6}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{1},wordgen::to_id{7}), 1);
		EXPECT_EQ(trans_rates(wordgen::from_id{1},wordgen::to_id{8}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{1},wordgen::to_id{9}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{1},wordgen::to_id{10}), 0);

		EXPECT_EQ(trans_rates(wordgen::from_id{4},wordgen::to_id{0}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{4},wordgen::to_id{1}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{4},wordgen::to_id{2}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{4},wordgen::to_id{3}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{4},wordgen::to_id{4}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{4},wordgen::to_id{5}), 1);
		EXPECT_EQ(trans_rates(wordgen::from_id{4},wordgen::to_id{6}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{4},wordgen::to_id{7}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{4},wordgen::to_id{8}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{4},wordgen::to_id{9}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{4},wordgen::to_id{10}), 0);

		EXPECT_EQ(trans_rates(wordgen::from_id{5},wordgen::to_id{0}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{5},wordgen::to_id{1}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{5},wordgen::to_id{2}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{5},wordgen::to_id{3}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{5},wordgen::to_id{4}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{5},wordgen::to_id{5}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{5},wordgen::to_id{6}), 1);
		EXPECT_EQ(trans_rates(wordgen::from_id{5},wordgen::to_id{7}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{5},wordgen::to_id{8}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{5},wordgen::to_id{9}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{5},wordgen::to_id{10}), 1);

		EXPECT_EQ(trans_rates(wordgen::from_id{10},wordgen::to_id{0}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{10},wordgen::to_id{1}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{10},wordgen::to_id{2}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{10},wordgen::to_id{3}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{10},wordgen::to_id{4}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{10},wordgen::to_id{5}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{10},wordgen::to_id{6}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{10},wordgen::to_id{7}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{10},wordgen::to_id{8}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{10},wordgen::to_id{9}), 0);
		EXPECT_EQ(trans_rates(wordgen::from_id{10},wordgen::to_id{10}), 0);
	}
}