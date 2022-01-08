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

	letter_groups.insert(wordgen::letter_group{" "});

	wordgen::load(argv[2], [&letter_groups](auto file) {
		wordgen::transition_rate_table transition_rates{std::size(letter_groups)};
		wordgen::stream_tokenizer tok{file};
		while(!tok.empty())
		{
			auto word = std::move(tok.front());
			tok.pop();
			auto word_split = split_longest(word, letter_groups);
			std::ranges::for_each(word_split, [from = letter_groups.get(" "), &letter_groups, &transition_rates](auto letter_group) mutable {
				auto to = letter_groups.get(letter_group);
				++transition_rates(wordgen::from_id{from.value()}, wordgen::to_id{to.value()});
				from = to;
			});

		}

		return transition_rates;
	});

	return 0;
}