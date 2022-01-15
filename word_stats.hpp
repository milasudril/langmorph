//@{"dependencies_extra":[{"ref":"pthread", "rel":"external", "origin": "system"}]}

#ifndef LANGMORPH_WORDSTATS_HPP
#define LANGMORPH_WORDSTATS_HPP

#include "./stat_counters.hpp"
#include "./letter_group_index.hpp"
#include "./input_queue.hpp"

#include <thread>
#include <numeric>

namespace langmorph
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
				process(word, letter_groups);
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

		void process(std::string_view word, letter_group_index const& letter_groups)
		{
			assert(std::size(letter_groups) == m_transition_rates.node_count());

			auto word_split = split_longest(word, letter_groups);
			if(std::size(word_split) < 3)
			{ return; }

			++m_length_hist(langmorph::histogram_index{std::size(word_split)});
			std::ranges::for_each(word_split,
								[from = letter_groups.get(" "),
								&letter_groups,
								&tr = m_transition_rates](auto letter_group) mutable {
				auto to = letter_groups.get(letter_group);
				++tr(langmorph::from_id{from.value()}, langmorph::to_id{to.value()});
				from = to;
			});
		}

		void process(std::span<std::string const> word, letter_group_index const& letter_groups)
		{
			assert(std::size(letter_groups) == m_transition_rates.node_count());
			std::ranges::for_each(word, [this, &letter_groups](auto const& item) {
				process(item, letter_groups);
			});
		}

	private:
		langmorph::histogram m_length_hist;
		transition_rate_table m_transition_rates;
	};

	word_stats operator+(word_stats&& a, word_stats const& b)
	{
		return std::move(a += b);
	}


	constexpr size_t num_workers = 15;

	std::vector<word_stats> create_word_stats(size_t size)
	{
		std::vector<word_stats> ret;
		std::generate_n(std::back_inserter(ret), num_workers, [size](){return word_stats{size};});
		return ret;
	}

	template<class TokenStream>
	word_stats load(std::type_identity<word_stats>, TokenStream&& words, letter_group_index const& letter_groups)
	{
		using buffer_type = std::vector<std::string>;
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
			buffer_type buffer;
			constexpr auto max_buffer_size = 4096/sizeof(std::string);
			buffer.reserve(max_buffer_size);
			while(!words.empty())
			{
				if(std::size(buffer) != max_buffer_size)
				{
					buffer.push_back(std::move(words.front()));
					words.pop();
				}
				else
				{
					fifos[thread_index % std::size(fifos)].push(std::move(buffer));
					++thread_index;
					buffer.push_back(std::move(words.front()));
					words.pop();
				}

				++wordcount;
			}

			if(std::size(buffer) != 0)
			{
				fifos[thread_index % std::size(fifos)].push(std::move(buffer));
			}

			fprintf(stderr, "\n Completed %zu\n", wordcount);

			for(size_t k = 0; k != std::size(fifos); ++k)
			{
				fifos[k].terminate();
			}
		}

		return std::accumulate(std::begin(stats), std::end(stats), word_stats{std::size(letter_groups)});
	}

	void store(word_stats const&, FILE*)
	{
	}
}

#endif