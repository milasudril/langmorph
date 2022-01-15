//@	{"target":{"name":"langmorph.o"}}

#include "./io_utils.hpp"
#include "./letter_group.hpp"
#include "./stream_tokenizer.hpp"
#include "./letter_group_index.hpp"
#include "./word_stats.hpp"
#include "./prob_distributions.hpp"

#include <random>

void show_help()
{
	puts(R"(Usage: langmorph <action> [command arguments]

Supported actions:

collect-stats  collects statistics given a file with words and a file with valid letter groups
make-words     generates a list of new words
)");
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		show_help();
		return 0;
	}

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
}