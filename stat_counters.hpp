#ifndef WORDGEN_STATCOUNTERS_HPP
#define WORDGEN_STATCOUNTERS_HPP

#include <memory>
#include <span>
#include <algorithm>

namespace wordgen
{
	class from_id
	{
	public:
		explicit from_id(size_t value):m_value{value}{}

		auto value() const { return m_value; }

	private:
		size_t m_value;
	};

	class to_id
	{
	public:
		explicit to_id(size_t value):m_value{value}{}

		auto value() const { return m_value; }

	private:
		size_t m_value;
	};

	class transition_rate_table
	{
	public:
		explicit transition_rate_table(size_t size):m_counts{std::make_unique<size_t[]>(size*size)}, m_size{size}
		{}

		size_t operator()(from_id from, to_id to) const
		{
			return m_counts[m_size*from.value() + to.value()];
		}

		size_t& operator()(from_id from, to_id to)
		{
			return m_counts[m_size*from.value() + to.value()];
		}

		std::span<size_t const> operator()(from_id from) const
		{
			return std::span{m_counts.get() + m_size*from.value(), m_size};
		}

		size_t node_count() const
		{
			return m_size;
		}

	private:
		std::unique_ptr<size_t[]> m_counts;
		size_t m_size;
	};

	class histogram_index
	{
	public:
		explicit histogram_index(size_t value):m_value{value}{}

		auto value() const { return m_value; }

		auto operator<=>(histogram_index const&) const = default;

	private:
		size_t m_value;
	};

	inline std::string to_string(histogram_index i)
	{ return std::to_string(i.value()); }

	class histogram
	{
	public:
		histogram(): m_size{0}
		{}

		size_t operator()(histogram_index index) const
		{
			return m_counts[index.value()];
		}

		size_t& operator()(histogram_index index)
		{
			auto const i_val = index.value();
			if(i_val >= m_size)
			{
				auto counts = std::make_unique<size_t[]>(i_val + 1);
				std::ranges::copy((*this)(), counts.get());
				m_counts = std::move(counts);
				m_size = i_val + 1;
			}
			return m_counts[i_val];
		}

		std::span<size_t const> operator()() const
		{
			return std::span{m_counts.get(), m_size};
		}

		auto largest_index() const
		{
			return histogram_index{m_size - 1};
		}

	private:
		std::unique_ptr<size_t[]> m_counts;
		size_t m_size;
	};
}

#endif