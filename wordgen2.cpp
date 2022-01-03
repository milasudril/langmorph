//@	{"target":{"name":"wordgen2.o"}}

#include <cstddef>
#include <array>
#include <cstring>
#include <cstdio>
#include <random>
#include <algorithm>

class transition_rates
{
public:
	transition_rates()
	{
		memset(&m_counts, 0, sizeof(m_counts));
	}

	auto& operator()(std::byte from, std::byte to)
	{
		return m_counts[static_cast<size_t>(from)][static_cast<size_t>(to)];
	}

	auto operator()(std::byte from, std::byte to) const
	{
		return m_counts[static_cast<size_t>(from)][static_cast<size_t>(to)];
	}

	auto const& rows() const { return m_counts; }

private:
	std::array<std::array<std::size_t, 256>, 256> m_counts;
};

transition_rates build_table(FILE* src)
{
	transition_rates ret;
	auto prev = static_cast<std::byte>(0x0);
	while(true)
	{
		auto ch_in = getc(src);
		if(ch_in == EOF)
		{
			return ret;
		}
		auto const val = static_cast<std::byte>(ch_in);
		++ret(prev, val);
		prev = val;
	}
}

class probablity_matrix
{
public:
	explicit probablity_matrix(transition_rates const& tr)
	{
		std::ranges::for_each(tr.rows(), [&probs = m_probabilities, k = static_cast<size_t>(0)](auto const& item) mutable{
			probs[k] = std::discrete_distribution<size_t>{std::begin(item), std::end(item)};
			++k;
		});
	}

	template<class Rng>
	std::byte operator()(std::byte prev, Rng& rng)
	{
		return static_cast<std::byte>(m_probabilities[static_cast<size_t>(prev)](rng));
	}

private:
	std::array<std::discrete_distribution<size_t>, 256> m_probabilities;
};

int main()
{
	auto probs = probablity_matrix{build_table(stdin)};
	std::mt19937 rng;
	auto prev = static_cast<std::byte>(0x41 + 32);
	for(size_t k = 0; k != 16384; ++k)
	{
		putchar(static_cast<char>(prev));
		prev = probs(prev, rng);
	}
}