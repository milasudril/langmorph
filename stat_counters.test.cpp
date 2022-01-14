//@{"target":{"name":"stat_counters.test", "actions":["run"]}}

#include "./stat_counters.hpp"

#include "testfwk/testfwk.hpp"

TESTCASE(langmorph_statcounters_from_id)
{
	langmorph::from_id val{1};
	EXPECT_EQ(val.value(), 1);
}

TESTCASE(langmorph_statcounters_to_id)
{
	langmorph::to_id val{1};
	EXPECT_EQ(val.value(), 1);
}

static_assert(!std::is_same_v<langmorph::to_id, langmorph::from_id>);

TESTCASE(langmorph_statcounters_transition_rate_table)
{
	langmorph::transition_rate_table tab{4};
	for(size_t k = 0; k != 4; ++k)
	{
		for(size_t l = 0; l!= 4; ++l)
		{
			tab(langmorph::from_id{k}, langmorph::to_id{l}) = k*4 + l;
		}
	}

	for(size_t k = 0; k != 4; ++k)
	{
		for(size_t l = 0; l!= 4; ++l)
		{
			EXPECT_EQ(tab(langmorph::from_id{k}, langmorph::to_id{l}), k*4 + l);
		}
	}

	for(size_t k = 0; k != 4; ++k)
	{
		auto row = tab(langmorph::from_id{k});
		EXPECT_EQ(std::size(row), 4);
		for(size_t l = 0; l!= std::size(row); ++l)
		{
			EXPECT_EQ(row[l], k*4 + l);
		}
	}
}

TESTCASE(langmorph_statcounters_transition_rate_table_sum)
{
	langmorph::transition_rate_table a{4};
	for(size_t k = 0; k != 4; ++k)
	{
		for(size_t l = 0; l!= 4; ++l)
		{
			a(langmorph::from_id{k}, langmorph::to_id{l}) = k*4 + l;
		}
	}

	langmorph::transition_rate_table b{4};
	for(size_t k = 0; k != 4; ++k)
	{
		for(size_t l = 0; l!= 4; ++l)
		{
			b(langmorph::from_id{k}, langmorph::to_id{l}) = k*4 + l;
		}
	}

	a += b;

	for(size_t k = 0; k != 4; ++k)
	{
		for(size_t l = 0; l!= 4; ++l)
		{
			EXPECT_EQ(a(langmorph::from_id{k}, langmorph::to_id{l}), 2*(k*4 + l));
		}
	}
}

TESTCASE(langmorph_statcounters_histogram_insert_values)
{
	langmorph::histogram hist;
	for(size_t k = 0; k != 4; ++k)
	{
		hist(langmorph::histogram_index{k}) = k;
	}

	for(size_t k = 0; k != 4; ++k)
	{
		EXPECT_EQ(hist(langmorph::histogram_index{k}), k);
	}

	auto values = hist();
	EXPECT_EQ(std::size(values), 4);
	for(size_t l = 0; l!= std::size(values); ++l)
	{
		EXPECT_EQ(values[l], l);
	}
}

TESTCASE(langmorph_statcounters_histogram_insert_values_sparse)
{
	langmorph::histogram hist;

	++hist(langmorph::histogram_index{453784});
	auto const vals = hist();
	EXPECT_EQ(std::all_of(std::begin(vals), std::end(vals) - 2, [](auto val){return val == 0;}), true);
	EXPECT_EQ(*(std::end(vals) - 1), 1);
}

TESTCASE(langmorph_statcounters_histogram_sum) {
	langmorph::histogram a;

	a(langmorph::histogram_index{0}) = 1;
	a(langmorph::histogram_index{1}) = 2;
	a(langmorph::histogram_index{2}) = 3;

	langmorph::histogram b;
	b(langmorph::histogram_index{0}) = 3;
	b(langmorph::histogram_index{1}) = 2;
	b(langmorph::histogram_index{2}) = 1;
	b(langmorph::histogram_index{3}) = 4;

	a += b;

	auto vals = a();

	EXPECT_EQ(std::size(vals), 4);
	EXPECT_EQ(vals[0], 4);
	EXPECT_EQ(vals[1], 4);
	EXPECT_EQ(vals[2], 4);
	EXPECT_EQ(vals[3], 4);
}