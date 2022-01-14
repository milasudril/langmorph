//@	{"target":{"name":"stream_tokenizer.test", "actions":["run"]}}

#include "./stream_tokenizer.hpp"
#include "./io_utils.hpp"

#include "testfwk/testfwk.hpp"

TESTCASE(langmorph_stream_tokenizer_read_data)
{
	auto handle = langmorph::create_file("testdata/input_file.txt", "rb");

	langmorph::stream_tokenizer tok{handle.get()};
	EXPECT_EQ(tok.empty(), false);
	std::vector<std::string> output;
	while(!tok.empty())
	{
		output.push_back(tok.front());
		tok.pop();
	}

	REQUIRE_EQ(std::size(output), 20);

	EXPECT_EQ(output[0], "some");
	EXPECT_EQ(output[1], "text");
	EXPECT_EQ(output[2], "with");
	EXPECT_EQ(output[3], "umläut");
	EXPECT_EQ(output[4], "https://www.youtube.com/watch?v=P0hSCKhySHc&list=PLoppF9lDpcn13_3bhfkYJVuCjNW_N2HEe");
	EXPECT_EQ(output[5], "A");
	EXPECT_EQ(output[6], "single");
	EXPECT_EQ(output[7], "should");
	EXPECT_EQ(output[8], "be");
	EXPECT_EQ(output[9], "ignored");
	EXPECT_EQ(output[10], "Multiple");
	EXPECT_EQ(output[11], "should");
	EXPECT_EQ(output[12], "be");
	EXPECT_EQ(output[13], "ignored");
	EXPECT_EQ(output[14], "This");
	EXPECT_EQ(output[15], "is");
	EXPECT_EQ(output[16], "a");
	EXPECT_EQ(output[17], "sentance");
	EXPECT_EQ(output[18], "More");
	EXPECT_EQ(output[19], "words");
}

