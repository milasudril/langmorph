#ifndef LANGMORPH_WORD_FACTORY_HPP
#define LANGMORPH_WORD_FACTORY_HPP

#include "./savestate.hpp"
#include "./prob_distributions.hpp"

#include <random>

namespace langmorph
{
	class word_factory
	{
	public:
		explicit word_factory(struct savestate&& savestate):
			m_word_length{gen_pmf(savestate.word_stats.length_histogram()())},
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
				std::string word{m_letter_groups.get(letter_group_id{current_group}).value()};
				while(true)
				{
					current_group = m_letter_group_probs.col(current_group, rng);
					if(current_group == 0)
					{
						break;
					}
					++actual_length;
					word += m_letter_groups.get(letter_group_id{current_group}).value();
				}
				if(actual_length == length)
				{	return word; }
			}
		}

	private:
		std::discrete_distribution<size_t> m_word_length;
		bivar_discrete_distribution m_letter_group_probs;
		letter_group_index m_letter_groups;
	};

	inline auto create(
		std::type_identity<word_factory>,
		std::string_view statefile,
		std::string_view statefile_entry
	)
	{
		return word_factory{load(std::type_identity<savestate>{}, statefile, statefile_entry)};
	}
}

#endif