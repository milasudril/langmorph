//@	{"target":{"name":"wordgen.o"}}

#include "./io_utils.hpp"
#include "./letter_group.hpp"
#include "./stream_tokenizer.hpp"
#include "./letter_group_index.hpp"
#include "./stat_counters.hpp"

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		puts("Too few arguments");
		return -1;
	}

	auto letter_groups = wordgen::load(argv[1], [](auto file) {
		return load(std::type_identity<wordgen::letter_group_index>{}, wordgen::stream_tokenizer{file});
	});



	wordgen::load(argv[2], [&letter_groups](auto file) {
		wordgen::transition_rate_table transition_rates{std::size(letter_groups)};
		wordgen::stream_tokenizer tok{file};
		while(!tok.empty())
		{
			auto word = std::move(tok.front());
			tok.pop();
			auto letter_group = next_group_longest(word, letter_groups);
			while(std::size(letter_group.second) != 0)
			{
				letter_group = next_group_longest(letter_group.second, letter_groups);
			}
		}

		return transition_rates;
	});

	return 0;
}