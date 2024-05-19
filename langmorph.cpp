//@	{"target":{"name":"langmorph.o"}}

#include "./savestate.hpp"
#include "./io_utils.hpp"
#include "./letter_group.hpp"
#include "./prob_distributions.hpp"
#include "./letter_group_file_resolver.hpp"
#include "./stream_tokenizer.hpp"

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
4. <location of langmorph>/../../share/langmorph

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
	puts(R"(Usage: langmorph make-words <stat file> <num words>

# The stat file

The stat file is expected to contain statistics on word lenght and letter group transtition rates.
use `langmorph collect-stats` to generate this file
)");
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

auto collect_stats(langmorph::letter_group_index const& letter_groups, std::span<std::string_view const> sources)
{
	langmorph::word_stats word_stats{std::size(letter_groups)};
	std::ranges::for_each(sources, [&word_stats, &letter_groups](auto file) {
		word_stats += langmorph::load(file, [&letter_groups](auto file) {
			return load(std::type_identity<langmorph::word_stats>{}, langmorph::stream_tokenizer{file}, letter_groups);
		});
	});

	return word_stats;
}

auto collect_stats(std::span<std::string_view const> sources, std::string_view letter_groups_file)
{
	auto letter_groups = load(langmorph::letter_group_file_resolver{letter_groups_file}, [](auto file) {
		return load(std::type_identity<langmorph::letter_group_index>{}, langmorph::stream_tokenizer{file});
	});
	auto word_stats = collect_stats(letter_groups, sources);
	return langmorph::savestate{
		.letter_groups = std::move(letter_groups),
		.word_stats = std::move(word_stats)
	};
}

void collect_stats(std::string_view statfile,
                   std::span<std::string_view const> sources,
                   std::string_view letter_groups_file)
{
	store(statfile, collect_stats(sources, letter_groups_file));
}

void collect_stats(std::string_view statfile, std::span<std::string_view const> sources)
{
	auto savestate = load(std::type_identity<langmorph::savestate>{}, statfile);
	savestate.word_stats += collect_stats(savestate.letter_groups, sources);
	store(statfile, savestate);
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

class word_factoroy
{
public:
	explicit word_factoroy(langmorph::savestate&& savestate):
		m_word_length{langmorph::gen_pmf(savestate.word_stats.length_histogram()())},
		m_letter_group_probs{savestate.word_stats.transition_rates()},
		m_letter_groups{std::move(savestate.letter_groups)}
	{}

	template<class Rng>
	std::string operator()(Rng&& rng)
	{
		auto const length = m_word_length(rng);
		while(true)
		{
			size_t actual_length = 0;
			auto current_group = m_letter_group_probs.col(0, rng);
			std::string word{m_letter_groups.get(langmorph::letter_group_id{current_group}).value()};
			while(true)
			{
				current_group = m_letter_group_probs.col(current_group, rng);
				if(current_group == 0)
				{
					break;
				}
				++actual_length;
				word += m_letter_groups.get(langmorph::letter_group_id{current_group}).value();
			}
			if(actual_length == length)
			{	return word; }
		}
	}

private:
	std::discrete_distribution<size_t> m_word_length;
	langmorph::bivar_discrete_distribution m_letter_group_probs;
	langmorph::letter_group_index m_letter_groups;
};

int make_words(std::span<std::string_view const> args)
{
	if(std::size(args) < 2)
	{
		puts(R"(Try langmorph help make-words)");
	}

	word_factoroy factory{load(std::type_identity<langmorph::savestate>{}, args[0])};
	auto const num_words = static_cast<size_t>(std::stoll(std::string{args[1]}));

	std::mt19937 rng;
	for(size_t k = 0; k != num_words; ++k)
	{
		puts(factory(rng).c_str());
	}

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
try
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
}
catch(std::exception const& e)
{
	fprintf(stderr, "%s\n", e.what());
	return -1;
}