//@{"target":{"name":"letter_group_file_resolver.test", "actions":["run"]}}

#include "./letter_group_file_resolver.hpp"

#include "testfwk/testfwk.hpp"

TESTCASE(langmorph_letter_group_file_resolver_empty_filename)
{
	langmorph::letter_group_file_resolver resolver{""};
	EXPECT_EQ(resolver.fetch_next(), false);
	auto const str = to_string(resolver);
	EXPECT_EQ(str, " ");
}

TESTCASE(langmorph_letter_group_file_resolver_abs_path)
{
	langmorph::letter_group_file_resolver resolver{"/foobar"};
	{
		auto const res = resolver.fetch_next();
		REQUIRE_EQ(res, true);
		EXPECT_EQ(resolver.current(), "/foobar");
	}

	{
		auto const str = to_string(resolver);
		EXPECT_EQ(str, " /foobar");
	}
}

TESTCASE(langmorph_letter_group_file_resolver_rel_path)
{
	langmorph::letter_group_file_resolver resolver{"./foobar"};
	{
		auto const res = resolver.fetch_next();
		REQUIRE_EQ(res, true);
		EXPECT_EQ(resolver.current(), "./foobar");
	}

	{
		auto const str = to_string(resolver);
		EXPECT_EQ(str, " ./foobar");
	}
}

TESTCASE(langmorph_letter_group_file_resolver_env)
{
	setenv("LANGMORPH_HOME", "kaka", 1);
	langmorph::letter_group_file_resolver resolver{"foobar"};
	{
		auto const res = resolver.fetch_next();
		REQUIRE_EQ(res, true);
		EXPECT_EQ(resolver.current(), "kaka/foobar");
	}

	{
		auto const res = resolver.fetch_next();
		EXPECT_EQ(res, false);
	}

	{
		auto const str = to_string(resolver);
		EXPECT_EQ(str, " kaka/foobar");
	}
}


TESTCASE(langmorph_letter_group_file_resolver_exe_dir)
{
	unsetenv("LANGMORPH_HOME");
	auto const expected_path = read_symlink(langmorph::fs::path{"/proc/self/exe"})
	                         .parent_path() / "share" / "langmorph" / "foobar";
	langmorph::letter_group_file_resolver resolver{"foobar"};
	{
		auto const res = resolver.fetch_next();
		REQUIRE_EQ(res, true);
		EXPECT_EQ(resolver.current(), expected_path);
	}

	{
		auto const str = to_string(resolver);
		EXPECT_EQ(str, std::string{" "}.append(expected_path.string()));
	}
}