//@	{"target":{"name":"stream_tokenizer.test", "actions":["run"]}}

#include "./stream_tokenizer.hpp"
#include "./io_utils.hpp"

#include "testfwk/testfwk.hpp"

TESTCASE(langmorph_stream_tokenizer_read_data)
{
	auto handle = langmorph::create_file("testdata/input_file.txt", "rb");
	REQUIRE_NE(handle.first.get(), nullptr);

	langmorph::stream_tokenizer tok{handle.first.get()};
	EXPECT_EQ(tok.empty(), false);
	std::vector<std::string> output;
	while(!tok.empty())
	{
		output.push_back(tok.front());
		tok.pop();
	}

	REQUIRE_EQ(std::size(output), 22);

	EXPECT_EQ(output[0], "some");
	EXPECT_EQ(output[1], "text");
	EXPECT_EQ(output[2], "with");
	EXPECT_EQ(output[3], "umläut");
	EXPECT_EQ(output[4], "https://www.youtube.com/watch?v=P0hSCKhySHc&list=PLoppF9lDpcn13_3bhfkYJVuCjNW_N2HEe");
	EXPECT_EQ(output[5], "A");
	EXPECT_EQ(output[6], "single");
	EXPECT_EQ(output[7], ".");
	EXPECT_EQ(output[8], "should");
	EXPECT_EQ(output[9], "not");
	EXPECT_EQ(output[10], "be");
	EXPECT_EQ(output[11], "ignored");
	EXPECT_EQ(output[12], "Multiple");
	EXPECT_EQ(output[13], "should");
	EXPECT_EQ(output[14], "be");
	EXPECT_EQ(output[15], "ignored");
	EXPECT_EQ(output[16], "This");
	EXPECT_EQ(output[17], "is");
	EXPECT_EQ(output[18], "a");
	EXPECT_EQ(output[19], "sentance");
	EXPECT_EQ(output[20], "More");
	EXPECT_EQ(output[21], "words");
}

