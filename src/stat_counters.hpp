#ifndef LANGMORPH_STATCOUNTERS_HPP
#define LANGMORPH_STATCOUNTERS_HPP

#include "./isqrt.hpp"

#include <memory>
#include <span>
#include <algorithm>
#include <cassert>

namespace langmorph
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
		using row_index_type = from_id;

		explicit transition_rate_table(size_t size):m_counts{std::make_unique<size_t[]>(size*size)}, m_size{size}
		{}

		template<class InputStream>
		explicit transition_rate_table(InputStream&& stream) requires (!std::same_as<std::decay_t<InputStream>, transition_rate_table> && !std::integral<InputStream>)
		{
			auto const size = std::size(stream);
			if(size % sizeof(size_t) != 0)
			{
				throw std::runtime_error{"Tried to load an invalid transition rate table"};
			}

			auto const num_entries = size/sizeof(size_t);
			auto buffer = std::make_unique<size_t[]>(num_entries);

			auto const node_count = isqrt(num_entries);
			if(node_count*node_count != num_entries)
			{
				throw std::runtime_error{"Tried to load an invalid transition rate table"};
			}

			stream.read(std::as_writable_bytes(std::span{buffer.get(), static_cast<size_t>(size)}));
			m_counts = std::move(buffer);
			m_size = node_count;
		}

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

		std::span<size_t const> operator()() const
		{
			return std::span{m_counts.get(), m_size*m_size};
		}

		size_t node_count() const
		{
			return m_size;
		}

		size_t row_count() const
		{
			return m_size;
		}

		size_t col_count() const
		{
			return m_size;
		}

		transition_rate_table& operator+=(transition_rate_table const& other)
		{
			assert(m_size == other.m_size);
			std::transform(m_counts.get(), m_counts.get() + m_size * m_size,
						   other.m_counts.get(), m_counts.get(), [](auto a, auto b) {
				return a + b;
			});
			return *this;
		}

	private:
		std::unique_ptr<size_t[]> m_counts;
		size_t m_size;
	};

	template<class OutputStream>
	void store(transition_rate_table const& obj, OutputStream&& stream)
	{
		stream.write(std::as_bytes(obj()));
	}

	template<class InputStream>
	auto load(std::type_identity<transition_rate_table>, InputStream&& stream)
	{
		return transition_rate_table{std::forward<InputStream>(stream)};
	}

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

		template<class InputStream>
		explicit histogram(InputStream&& stream) requires (!std::same_as<std::decay_t<InputStream>, histogram>)
		{
			auto const size = std::size(stream);
			if(size % sizeof(size_t) != 0)
			{
				throw std::runtime_error{"Tried to load an invalid histogram"};
			}

			auto const num_entries = size/sizeof(size_t);
			auto buffer = std::make_unique<size_t[]>(num_entries);
			stream.read(std::as_writable_bytes(std::span{buffer.get(), static_cast<size_t>(size)}));
			m_counts = std::move(buffer);
			m_size = num_entries;
		}

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

		histogram& operator+=(histogram const& other)
		{
			for(size_t k = 0; k != other.m_size; ++k)
			{
				(*this)(histogram_index{k}) += other.m_counts[k];
			}
			return *this;
		}

	private:
		std::unique_ptr<size_t[]> m_counts;
		size_t m_size;
	};

	template<class OutputStream>
	void store(histogram const& obj, OutputStream&& stream)
	{
		stream.write(std::as_bytes(obj()));
	}

	template<class InputStream>
	auto load(std::type_identity<histogram>, InputStream&& stream)
	{
		return histogram{std::forward<InputStream>(stream)};
	}
}
#endif