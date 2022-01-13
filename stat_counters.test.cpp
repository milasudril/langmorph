//@{"target":{"name":"stat_counters.test", "actions":["run"]}}

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

TESTCASE(wordgen_statcounters_transition_rate_table_sum)
{
	wordgen::transition_rate_table a{4};
	for(size_t k = 0; k != 4; ++k)
	{
		for(size_t l = 0; l!= 4; ++l)
		{
			a(wordgen::from_id{k}, wordgen::to_id{l}) = k*4 + l;
		}
	}

	wordgen::transition_rate_table b{4};
	for(size_t k = 0; k != 4; ++k)
	{
		for(size_t l = 0; l!= 4; ++l)
		{
			b(wordgen::from_id{k}, wordgen::to_id{l}) = k*4 + l;
		}
	}

	a += b;

	for(size_t k = 0; k != 4; ++k)
	{
		for(size_t l = 0; l!= 4; ++l)
		{
			EXPECT_EQ(a(wordgen::from_id{k}, wordgen::to_id{l}), 2*(k*4 + l));
		}
	}
}

TESTCASE(wordgen_statcounters_histogram_insert_values)
{
	wordgen::histogram hist;
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

TESTCASE(wordgen_statcounters_histogram_insert_values_sparse)
{
	wordgen::histogram hist;

	++hist(wordgen::histogram_index{453784});
	auto const vals = hist();
	EXPECT_EQ(std::all_of(std::begin(vals), std::end(vals) - 2, [](auto val){return val == 0;}), true);
	EXPECT_EQ(*(std::end(vals) - 1), 1);
}

TESTCASE(wordgen_statcounters_histogram_sum) {
	wordgen::histogram a;

	a(wordgen::histogram_index{0}) = 1;
	a(wordgen::histogram_index{1}) = 2;
	a(wordgen::histogram_index{2}) = 3;

	wordgen::histogram b;
	b(wordgen::histogram_index{0}) = 3;
	b(wordgen::histogram_index{1}) = 2;
	b(wordgen::histogram_index{2}) = 1;
	b(wordgen::histogram_index{3}) = 4;

	a += b;

	auto vals = a();

	EXPECT_EQ(std::size(vals), 4);
	EXPECT_EQ(vals[0], 4);
	EXPECT_EQ(vals[1], 4);
	EXPECT_EQ(vals[2], 4);
	EXPECT_EQ(vals[3], 4);
}