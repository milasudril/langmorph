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
	puts(R"(Usage: langmorph collect-stats <stat file> [letter group file] -- <source> [sources...]

# The stat file

This file is used to store statistics. If a letter group file is specified, the file will be
stat file will be recreated, otherwise statistics will be accumulated.

# The Letter group file

The letter group file is a list of valid letter groups. If not specified, the letter groups will be
read from <stat file>. Langmorph uses the following rules to locate the letter group file:

1. If the path is absoulte, that path will be used
2. If the name begins with `.`, the path is expected to be relative to the current
3. The directory specified by the LANGMORPH_HOME environment variable
4. <location of langmorph>/../share/langmorph

On a UNIX system, langmorph would typically be located in /usr/bin

# Source files

Source files are used to collect statistics. Each word in a source file is tokenized based on the
largest match in the lettergroup file. If the letter group file contains a, c, e, g, h, l, r, s,
and sch, the word schlager, will be interpreted as sch-l-a-g-e-r. Statistics are collected for the
number of tokens in different words, and token succuessors. In this case, there would be one
incidence of an l that follows sch.
)");
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

namespace langmorph
{
	class letter_group_file_resolver
	{
	public:
		explicit letter_group_file_resolver(fs::path&& filename):m_filename{std::move(filename)},m_alt{0}
		{
		}

		bool fetch_next()
		{
			if(m_alt == 4)
			{return false;}

			++m_alt;
			return true;
		}

		fs::path const& current() const
		{
			return m_tried_paths.back();
		}

		std::span<fs::path const> tried_paths() const
		{
			return m_tried_paths;
		}

	private:
		fs::path m_filename;
		std::vector<fs::path> m_tried_paths;
		size_t m_alt;
	};

	std::string to_string(letter_group_file_resolver const& res)
	{
		std::string ret;
		std::ranges::for_each(res.tried_paths(), [&ret](auto const& item){
			ret.append(" ").append(item);
		});
		return ret;
	}
}

void collect_stats(std::string_view statfile,
                   std::span<std::string_view const> sources,
                   std::string_view letter_groups_file)
{
	auto letter_groups = load(langmorph::letter_group_file_resolver{letter_groups_file}, [](auto file) {
		return load(std::type_identity<langmorph::letter_group_index>{}, langmorph::stream_tokenizer{file});
	});
	printf("statfile: %s\n", std::data(statfile));
	printf("sources:\n");
	std::ranges::for_each(sources, [](auto item){
		printf("    %s\n", std::data(item));
	});
	printf("letter_group_file: %s\n", std::data(letter_groups_file));
}

void collect_stats(std::string_view statfile, std::span<std::string_view const> sources)
{
	printf("statfile: %s\n", std::data(statfile));
	printf("sources:\n");
	std::ranges::for_each(sources, [](auto item){
		printf("    %s\n", std::data(item));
	});
}

int collect_stats(std::span<std::string_view const> args)
{
	if(std::size(args) < 3)
	{
		puts(R"(Try langmorph help collect-stats)");
		return -1;
	}

	auto const has_letter_group_file = args[1] != "--";
	if(has_letter_group_file && std::size(args) < 4)
	{
		puts(R"(Try langmorph help collect-stats)");
		return -1;
	}

	auto const statfile = args[0];
	auto const letter_group_file = has_letter_group_file ? args[1] : std::string_view{};
	auto const sources = std::span{std::begin(args) + (has_letter_group_file? 3 : 2), std::end(args)};

	if(has_letter_group_file)
	{
		collect_stats(statfile, sources, letter_group_file);
	}
	else
	{
		collect_stats(statfile, sources);
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
	std::transform(argv + std::min(argc, 2), argv + argc, std::back_inserter(ret), [](auto item){
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