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

