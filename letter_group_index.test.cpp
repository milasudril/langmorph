//@	{"target":{"name":"letter_group_index.test", "actions":["run"]}}

#include "./letter_group_index.hpp"

#include "testfwk/testfwk.hpp"

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