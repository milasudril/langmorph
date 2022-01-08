#ifndef WORDGEN_LETTERGROUPINDEX_HPP
#define WORDGEN_LETTERGROUPINDEX_HPP

#include "./letter_group.hpp"

#include <assert>

namespace wordgen
{
	class letter_group_id
	{
	public:
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

	private:
		size_t m_value;
	}

	class letter_group_index
	{
	public:
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

		letter_group const& get(letter_group_id id) const
		{
			assert(m_id_to_group.contains(id))
			return m_id_to_group.find(id)->second;
		}

	private:
		std::map<letter_group_id, letter_group> m_id_to_group;
		std::map<std::sting_view<letter_group>, letter_group_id> m_group_to_id;
		letter_group_id m_last_id;
	};
}

#endif