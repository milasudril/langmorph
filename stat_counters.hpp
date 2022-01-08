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
		{
			std::fill(m_counts.get(), m_counts.get() + size*size, 0);
		}

		size_t operator()(from_id from, to_id to) const
		{
			return m_counts[m_size*from.value()+ to.value()];
		}

		size_t& operator()(from_id from, to_id to)
		{
			return m_counts[m_size*from.value()+ to.value()];
		}

		std::span<size_t const> operator()(from_id from) const
		{
			return std::span{m_counts.get() + m_size*from.value(), m_size};
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

	private:
		size_t m_value;
	};

	class histogram
	{
	public:
		explicit histogram(size_t size):m_counts{std::make_unique<size_t[]>(size)}, m_size{size}
		{
			std::fill(m_counts.get(), m_counts.get() + size, 0);
		}

		size_t operator()(histogram_index index) const
		{
			return m_counts[index.value()];
		}

		size_t& operator()(histogram_index index)
		{
			return m_counts[index.value()];
		}

		std::span<size_t const> operator()() const
		{
			return std::span{m_counts.get(), m_size};
		}

	private:
		std::unique_ptr<size_t[]> m_counts;
		size_t m_size;
	};
}

#endif