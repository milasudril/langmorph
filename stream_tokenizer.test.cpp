//@	{"target":{"name":"stream_tokenizer.test", "actions":["run"]}}

#include "./stream_tokenizer.hpp"
#include "./io_utils.hpp"

#include "testfwk/testfwk.hpp"

TESTCASE(stream_tokenizer_read_data)
{
	auto handle = wordgen::create_file("testdata/input_file.txt", "rb");

	wordgen::stream_tokenizer tok{handle.get()};
	EXPECT_EQ(tok.empty(), false);
	std::vector<std::string> output;
	while(!tok.empty())
	{
		output.push_back(tok.top());
		tok.pop();
	}

	REQUIRE_EQ(std::size(output), 11);

	EXPECT_EQ(output[0], "some");
	EXPECT_EQ(output[1], "text");
	EXPECT_EQ(output[2], "with");
	EXPECT_EQ(output[3], "umläut");
	EXPECT_EQ(output[4], "https://www.youtube.com/watch?v=P0hSCKhySHc&list=PLoppF9lDpcn13_3bhfkYJVuCjNW_N2HEe");
	EXPECT_EQ(output[5], "This");
	EXPECT_EQ(output[6], "is");
	EXPECT_EQ(output[7], "a");
	EXPECT_EQ(output[8], "sentance");
	EXPECT_EQ(output[9], "More");
	EXPECT_EQ(output[10], "words");
}
