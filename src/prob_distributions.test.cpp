//@	{"target":{"name":"prob_distributions.test", "actions":["run"]}}

#include "./prob_distributions.hpp"

#include "testfwk/testfwk.hpp"

TESTCASE(langmorph_gen_pmf_all_zeros)
{
	std::array<size_t, 4> counts{0, 0, 0, 0};

	auto pmf = langmorph::gen_pmf(counts);

	auto weights = pmf.probabilities();
	REQUIRE_EQ(std::size(weights), 4);
	EXPECT_EQ(weights[0], 0.25);
	EXPECT_EQ(weights[1], 0.25);
	EXPECT_EQ(weights[2], 0.25);
	EXPECT_EQ(weights[3], 0.25);
}

TESTCASE(langmorph_gen_pmf_one_zero)
{
	std::array<size_t, 4> counts{1, 2, 1, 0};

	auto pmf = langmorph::gen_pmf(counts);

	auto weights = pmf.probabilities();
	REQUIRE_EQ(std::size(weights), 4);
	EXPECT_EQ(weights[0], 0.25);
	EXPECT_EQ(weights[1], 0.5);
	EXPECT_EQ(weights[2], 0.25);
	EXPECT_EQ(weights[3], 0.0);
}

namespace
{
	struct count_source
	{
		using row_index_type = size_t;
		size_t row_count() const { return 2; }

		std::span<size_t const> operator()(size_t k)
		{
			return rows[k];
		}

		std::array<std::array<size_t, 4>, 2> rows;
	};
}

TESTCASE(langmorph_bivar_probability_distribution)
{
	count_source src;
	src.rows[0] = std::array<size_t, 4>{1, 2, 1, 0};
	src.rows[1] = std::array<size_t, 4>{0, 0, 0};

	langmorph::bivar_discrete_distribution obj{src};
	REQUIRE_EQ(obj.row_count(), 2);

	auto const weights_1 = obj.probabilities(0);
	REQUIRE_EQ(std::size(weights_1), 4);
	EXPECT_EQ(weights_1[0], 0.25);
	EXPECT_EQ(weights_1[1], 0.5);
	EXPECT_EQ(weights_1[2], 0.25);
	EXPECT_EQ(weights_1[3], 0);

	auto const weights_2 = obj.probabilities(1);
	REQUIRE_EQ(std::size(weights_2), 4);
	EXPECT_EQ(weights_2[0], 0.25);
	EXPECT_EQ(weights_2[1], 0.25);
	EXPECT_EQ(weights_2[2], 0.25);
	EXPECT_EQ(weights_2[3], 0.25);

	std::default_random_engine rng;
	obj.col(0, rng);
	obj.col(1, rng);
}