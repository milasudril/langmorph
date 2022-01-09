//@	{"target":{"name":"wordstats.test", "actions":["run"]}}

#include "./wordstats.hpp"

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
	words.push("schlager");
	words.push("hej");
	words.push("är");

	wordgen::word_stats stats{words, load(std::type_identity<wordgen::letter_group_index>{}, letter_groups_in)};
}