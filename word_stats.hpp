//@{"dependencies_extra":[{"ref":"pthread", "rel":"external", "origin": "system"}]}

#ifndef WORDGEN_WORDSTATS_HPP
#define WORDGEN_WORDSTATS_HPP

#include "./stat_counters.hpp"
#include "./letter_group_index.hpp"
#include "./input_queue.hpp"

#include <thread>
#include <numeric>

namespace wordgen
{
	class word_stats
	{
	public:
		template<class TokenStream>
		explicit word_stats(TokenStream&& words, letter_group_index const& letter_groups):
			m_transition_rates{std::size(letter_groups)}
		{
			size_t wordcount = 0;
			while(!words.empty())
			{
				auto word = std::move(words.front());
				words.pop();
				++wordcount;
			//	for(size_t k = 0; k != std::size(word); ++k)
				{
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
			fprintf(stderr, "%zu\n", wordcount);
		}

		explicit word_stats(size_t transition_rates_size):m_transition_rates{transition_rates_size}{}

		auto const& length_histogram() const
		{ return m_length_hist; }

		auto const& transition_rates() const
		{ return m_transition_rates; }

		word_stats& operator+=(word_stats const& other)
		{
			m_length_hist += other.m_length_hist;
			m_transition_rates += other.m_transition_rates;
			return *this;
		}

	private:
		wordgen::histogram m_length_hist;
		transition_rate_table m_transition_rates;
	};

	word_stats operator+(word_stats&& a, word_stats const& b)
	{
		return std::move(a += b);
	}


	constexpr size_t num_workers = 16;

	std::vector<word_stats> create_word_stats(size_t size)
	{
		std::vector<word_stats> ret;
		std::generate_n(std::back_inserter(ret), num_workers, [size](){return word_stats{size};});
		return ret;
	}

	template<class TokenStream>
	word_stats load(std::type_identity<word_stats>, TokenStream&& words, letter_group_index const& letter_groups)
	{
		using buffer_type = std::string;
		std::array<input_queue<buffer_type>, num_workers> fifos;
		auto stats = create_word_stats(std::size(letter_groups));

		{
			std::array<std::jthread, num_workers> workers;
			for(size_t k = 0; k != std::size(workers); ++k)
			{
				workers[k] = std::jthread([&fifo = fifos[k], &letter_groups, &stat = stats[k]](){
					stat += word_stats{fifo, letter_groups};
				});
			}

			size_t thread_index = 0;
			size_t wordcount = 0;
			while(!words.empty())
			{
				fifos[thread_index % std::size(workers)].push(std::move(words.front()));
				++thread_index;
				words.pop();
#if 0
				if(wordcount % (1024*1024*4) == 0)
				{
					fprintf(stderr, "\rwordcount: %zu", wordcount);
					fflush(stderr);
				}
#endif
				++wordcount;
			}

			fprintf(stderr, "\n Completed %zu\n", wordcount);
#if 1
			for(size_t k = 0; k != std::size(fifos); ++k)
			{
				fifos[k].terminate();
			}
#endif
		}

		return std::accumulate(std::begin(stats), std::end(stats), word_stats{std::size(letter_groups)});
	}
}

#endif