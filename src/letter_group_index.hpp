#ifndef LANGMORPH_LETTERGROUPINDEX_HPP
#define LANGMORPH_LETTERGROUPINDEX_HPP

#include "./letter_group.hpp"

#include <cassert>
#include <map>
#include <algorithm>
#include <cstdio>

namespace langmorph
{
	class letter_group_id
	{
	public:
		explicit letter_group_id(size_t value):m_value{value}{}

		letter_group_id():m_value{0}{}

		letter_group_id& operator++()
		{
			++m_value;
			return *this;
		}

		letter_group_id operator++(int)
		{
			auto ret = *this;
			++m_value;
			return ret;
		}

		auto operator<=>(letter_group_id const&) const = default;

		auto value() const { return m_value; }

	private:
		size_t m_value;
	};

	inline auto to_string(letter_group_id id)
	{
		return std::to_string(id.value());
	}

	class letter_group_index
	{
	public:
		letter_group_index()
		{
			insert(letter_group{" "});
		}

		void insert(letter_group&& group)
		{
			if(m_group_to_id.contains(group.value()))
			{ return; }

			auto ip = m_id_to_group.insert(std::pair{m_last_id, std::move(group)});
			m_group_to_id.insert(std::pair{ip.first->second.value(), m_last_id});
			++m_last_id;
		}

		letter_group_id get(letter_group const& group) const
		{
			assert(m_group_to_id.contains(group.value()));
			return m_group_to_id.find(group.value())->second;
		}

		letter_group_id get(std::string_view group) const
		{
			assert(m_group_to_id.contains(group));
			return m_group_to_id.find(group)->second;
		}


		letter_group const& get(letter_group_id id) const
		{
			assert(m_id_to_group.contains(id));
			return m_id_to_group.find(id)->second;
		}

		letter_group& get(letter_group_id id)
		{
			assert(m_id_to_group.contains(id));
			return m_id_to_group.find(id)->second;
		}

		bool contains(letter_group const& group) const
		{
			return m_group_to_id.contains(group.value());
		}

		bool contains(std::string_view group) const
		{
			return m_group_to_id.contains(group);
		}


		bool contains(letter_group_id id) const
		{
			return m_id_to_group.contains(id);
		}

		size_t size() const
		{
			return std::size(m_id_to_group);
		}

		auto const& by_id() const
		{
			return m_id_to_group;
		}

		auto const& by_name() const
		{
			return m_group_to_id;
		}

		void erase(letter_group_id id)
		{
			auto const group = m_id_to_group.find(id);
			if(group == std::end(m_id_to_group))
			{ return; }

			m_group_to_id.erase(group->second.value());
			m_id_to_group.erase(group);
		}

	private:
		std::map<letter_group_id, letter_group> m_id_to_group;
		std::map<std::string_view, letter_group_id> m_group_to_id;
		letter_group_id m_last_id;
	};

	inline void store(letter_group_index const& letter_groups, FILE* f)
	{
		std::ranges::for_each(letter_groups.by_id(), [f](auto const& item){
			fprintf(f, "%s\n", item.second.c_str());
		});
	}
}

#endif