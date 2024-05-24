//@	{"target":{"name":"isqrt.test", "actions":["run"]}}

#include "./isqrt.hpp"

#include "testfwk/testfwk.hpp"
#include <limits>

TESTCASE(langmorph_isqrt_uintmax)
{
	auto val = langmorph::isqrt(std::numeric_limits<unsigned int>::max());
	EXPECT_EQ(val, 65535);
}

TESTCASE(langmorph_isqrt_perfect_square_neighbour)
{
	EXPECT_EQ(langmorph::isqrt(24u), 4u);
	EXPECT_EQ(langmorph::isqrt(25u), 5u);
	EXPECT_EQ(langmorph::isqrt(26u), 5u);
}

TESTCASE(langmorph_isqrt_zero)
{
	EXPECT_EQ(langmorph::isqrt(0u), 0u);
}

TESTCASE(langmorph_isqrt_one)
{
	EXPECT_EQ(langmorph::isqrt(1u), 1u);
}

TESTCASE(langmorph_isqrt_intmax)
{
	auto val = langmorph::isqrt(std::numeric_limits<int>::max());
	EXPECT_EQ(val, 46340);
}

TESTCASE(langmorph_isqrt_sizet_max)
{
	auto val = langmorph::isqrt(std::numeric_limits<size_t>::max());
	EXPECT_EQ(val, 0xffffffff);
}