//@{"target":{"name":"stat_counters.test.cpp", "actions":["run"]}}

#include "./stat_counters.hpp"

#include "testfwk/testfwk.hpp"

TESTCASE(wordgen_statcounters_from_id)
{
	wordgen::from_id val{1};
	EXPECT_EQ(val.value(), 1);
}

TESTCASE(wordgen_statcounters_to_id)
{
	wordgen::to_id val{1};
	EXPECT_EQ(val.value(), 1);
}

static_assert(!std::is_same_v<wordgen::to_id, wordgen::from_id>);

TESTCASE(wordgen_statcounters_transition_rate_table)
{
	wordgen::transition_rate_table tab{4};
	for(size_t k = 0; k != 4; ++k)
	{
		for(size_t l = 0; l!= 4; ++l)
		{
			tab(wordgen::from_id{k}, wordgen::to_id{l}) = k*4 + l;
		}
	}

	for(size_t k = 0; k != 4; ++k)
	{
		for(size_t l = 0; l!= 4; ++l)
		{
			EXPECT_EQ(tab(wordgen::from_id{k}, wordgen::to_id{l}), k*4 + l);
		}
	}

	for(size_t k = 0; k != 4; ++k)
	{
		auto row = tab(wordgen::from_id{k});
		EXPECT_EQ(std::size(row), 4);
		for(size_t l = 0; l!= std::size(row); ++l)
		{
			EXPECT_EQ(row[l], k*4 + l);
		}
	}
}

TESTCASE(wordgen_statcounters_histogram)
{
	wordgen::histogram hist{4};
	for(size_t k = 0; k != 4; ++k)
	{
		hist(wordgen::histogram_index{k}) = k;
	}

	for(size_t k = 0; k != 4; ++k)
	{
		EXPECT_EQ(hist(wordgen::histogram_index{k}), k);
	}

	auto values = hist();
	EXPECT_EQ(std::size(values), 4);
	for(size_t l = 0; l!= std::size(values); ++l)
	{
		EXPECT_EQ(values[l], l);
	}
}