//@	{"target":{"name":"wordgen.o"}}

#include "./io_utils.hpp"
#include "./letter_group.hpp"
#include "./stream_tokenizer.hpp"
#include "./letter_group_index.hpp"
#include "./word_stats.hpp"
#include "./prob_distributions.hpp"

#include <random>

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		puts("Too few arguments");
		return -1;
	}

	auto letter_groups = wordgen::load(argv[1], [](auto file) {
		return load(std::type_identity<wordgen::letter_group_index>{}, wordgen::stream_tokenizer{file});
	});

	auto word_stats = wordgen::load(argv[2], [&letter_groups](auto file) {
		return load(std::type_identity<wordgen::word_stats>{}, wordgen::stream_tokenizer{file}, letter_groups);
	});

	auto word_length = wordgen::gen_pmf(word_stats.length_histogram()());
	wordgen::bivar_discrete_distribution letter_group_probs{word_stats.transition_rates()};

	std::mt19937 rng;

	for(size_t k = 0; k != 1024; ++k)
	{
		auto length = word_length(rng);
		auto current_group = letter_group_probs.col(0, rng);
		std::string word{letter_groups.get(wordgen::letter_group_id{current_group}).value()};
		assert(length != 0);
		for(size_t l = 1; l != length; ++l)
		{
			current_group = letter_group_probs.col(current_group, rng);
			word += letter_groups.get(wordgen::letter_group_id{current_group}).value();
		}
		puts(word.c_str());
	}

	return 0;
}