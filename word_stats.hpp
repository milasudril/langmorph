//@{"dependencies_extra":[{"ref":"pthread", "rel":"external", "origin": "system"}]}

#ifndef WORDGEN_WORDSTATS_HPP
#define WORDGEN_WORDSTATS_HPP

#include "./stat_counters.hpp"
#include "./letter_group_index.hpp"
#include "./input_queue.hpp"

#include <thread>

namespace wordgen
{
	class word_stats
	{
	public:
		template<class TokenStream>
		explicit word_stats(TokenStream&& words, letter_group_index const& letter_groups):
			m_transition_rates{std::size(letter_groups)}
		{
			while(!words.empty())
			{
				auto word = std::move(words.front());
				words.pop();
				auto word_split = split_longest(word, letter_groups);
				if(std::size(word_split) < 3)
				{ continue; }

				++m_length_hist(wordgen::histogram_index{std::size(word_split)});
				std::ranges::for_each(word_split,
					                  [from = letter_groups.get(" "),
					                   &letter_groups,
					                   &tr = m_transition_rates](auto letter_group) mutable {
					auto to = letter_groups.get(letter_group);
					++tr(wordgen::from_id{from.value()}, wordgen::to_id{to.value()});
					from = to;
				});
			}
		}

		auto const& length_histogram() const
		{ return m_length_hist; }

		auto const& transition_rates() const
		{ return m_transition_rates; }

	private:
		wordgen::histogram m_length_hist;
		transition_rate_table m_transition_rates;
	};

	template<class TokenStream>
	void load(std::type_identity<word_stats>, TokenStream&& words, letter_group_index const& letter_groups)
	{
		std::array<std::jthread, 16> workers;
		std::array<input_queue<std::string>, 16> fifos;
		for(size_t k = 0; k != std::size(workers); ++k)
		{
			workers[k] = std::jthread([&fifo = fifos[k], &letter_groups](){
				word_stats stats{fifo, letter_groups};
			});
		}

		size_t wordcount = 0;
		while(!words.empty())
		{
			fifos[wordcount % 16].push(std::move(words.front()));
			words.pop();
			++wordcount;
		}

		for(size_t k = 0; k != std::size(fifos); ++k)
		{
			fifos[k].terminate();
		}
	}
}

#endif