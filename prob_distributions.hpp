#ifndef WORDGEN_PROBDISTRIBUTIONS_HPP
#define WORDGEN_PROBDISTRIBUTIONS_HPP

#include <random>
#include <algorithm>
#include <span>

namespace wordgen
{
	auto gen_pmf(std::span<size_t const> counts)
	{
		if(std::ranges::all_of(counts, [](auto val){return val == 0;}))
		{
			return std::discrete_distribution<size_t>{std::size(counts), 0.0, 1.0, [](auto){return 1.0;}};
		}

		return std::discrete_distribution<size_t>{std::begin(counts), std::end(counts)};
	}

	class bivar_discrete_distribution
	{
	public:
		template<class Source>
		explicit bivar_discrete_distribution(Source&& src) requires
			(!std::same_as<bivar_discrete_distribution, std::decay_t<Source>>):
			m_distributions{std::make_unique<std::discrete_distribution<size_t>[]>(src.row_count())},
			m_size{src.row_count()}
		{
			using row_index_type = typename std::decay_t<Source>::row_index_type;

			for(size_t k = 0; k != src.row_count(); ++k)
			{
				m_distributions[k] = gen_pmf(src(row_index_type{k}));
			}
		}

		template<class Rng>
		size_t col(size_t row, Rng&& rng)
		{
			assert(row < m_size);
			return m_distributions[row](rng);
		}

		decltype(auto) probabilities(size_t row) const
		{
			assert(row < m_size);
			return m_distributions[row];
		}

		size_t row_count() const
		{
			return m_size;
		}

	private:
		std::unique_ptr<std::discrete_distribution<size_t>[]> m_distributions;
		size_t m_size;
	};
}

#endif