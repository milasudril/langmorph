//@	{"target":{"name":"langmorph.o"}}

#include "./io_utils.hpp"
#include "./letter_group.hpp"
#include "./stream_tokenizer.hpp"
#include "./letter_group_index.hpp"
#include "./word_stats.hpp"
#include "./prob_distributions.hpp"

#include <random>

int show_help_general()
{
	puts(R"(Usage: langmorph <action> [command arguments]

Supported actions:

collect-stats  collects statistics given a file with words and a file with valid letter groups
make-words     generates a list of new words
)");
	return 0;
}

int show_help_collect_stats()
{
	return 0;
}

int show_help_make_words()
{
	return 0;
}

int show_help(std::string_view cmd)
{
	if(cmd == "collect-stats")
	{
		return show_help_collect_stats();
	}

	if(cmd == "make-words")
	{
		return show_help_make_words();
	}
	return show_help_general();
}

int show_help(std::span<std::string_view const> args)
{
	if(std::size(args) == 0)
	{
		return show_help_general();
	}
	return show_help(args[0]);
}

int collect_stats(std::span<std::string_view const> args)
{
	if(std::size(args) < 2)
	{
		puts(R"(collect-stats requires one letter group file, together with at least one source file to analyze. Try

langmorph help collect-stats
)");
		return -1;
	}
	return 0;
}

int make_words(std::span<std::string_view const>)
{
	puts("Make words");
	return 0;
}

std::vector<std::string_view> get_action_args(int argc, char const* const* argv)
{
	std::vector<std::string_view> ret;
	std::transform(argv + 2, argv + argc, std::back_inserter(ret), [](auto item){
		return std::string_view{item};
	});
	return ret;
}

std::string_view get_action(int argc, char const* const* argv)
{
	if(argc < 2)
	{
		return std::string_view{"help"};
	}

	return std::string_view{argv[1]};
}

int main(int argc, char** argv)
{
	auto action = get_action(argc, argv);
	auto action_args = get_action_args(argc, argv);
	if(action == "help")
	{
		return show_help(action_args);
	}

	if(action == "collect-stats")
	{
		return collect_stats(action_args);
	}

	if(action == "make-words")
	{
		return make_words(action_args);
	}

	return show_help(action_args);
#if 0

	auto letter_groups = langmorph::load(argv[1], [](auto file) {
		return load(std::type_identity<langmorph::letter_group_index>{}, langmorph::stream_tokenizer{file});
	});

	auto word_stats = langmorph::load(argv[2], [&letter_groups](auto file) {
		return load(std::type_identity<langmorph::word_stats>{}, langmorph::stream_tokenizer{file}, letter_groups);
	});

	auto word_length = langmorph::gen_pmf(word_stats.length_histogram()());
	langmorph::bivar_discrete_distribution letter_group_probs{word_stats.transition_rates()};

	std::mt19937 rng;

	for(size_t k = 0; k != 1024; ++k)
	{
		auto length = word_length(rng);
		auto current_group = letter_group_probs.col(0, rng);
		std::string word{letter_groups.get(langmorph::letter_group_id{current_group}).value()};
		assert(length != 0);
		for(size_t l = 1; l != length; ++l)
		{
			current_group = letter_group_probs.col(current_group, rng);
			word += letter_groups.get(langmorph::letter_group_id{current_group}).value();
		}
		puts(word.c_str());
	}

	return 0;
#endif
}