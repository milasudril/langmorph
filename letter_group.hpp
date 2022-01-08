#ifndef WORDGEN_LETTERGROUP_HPP
#define WORDGEN_LETTERGROUP_HPP

#include <string>

namespace wordgen
{
	class letter_group
	{
	public:
		explicit letter_group(std::string&& str):m_chars{std::move(str)}{}

		std::string_view value() const
		{
			return m_chars;
		}

		bool operator==(letter_group const&) const = default;

	private:
		std::string m_chars;
	};

	inline auto to_string(letter_group const& group)
	{
		return std::string{group.value()};
	}

	template<class LetterGroupSet>
	std::pair<std::string_view, std::string_view>
	next_group_shortest(std::string_view word, LetterGroupSet const& letter_groups)
	{
		auto ptr = std::begin(word);
		while(ptr != std::end(word))
		{
			++ptr;
			auto const slice = std::string_view{std::begin(word), ptr};
			if(auto i = letter_groups.find(slice); i != std::end(letter_groups))
			{
				return std::pair{slice, std::string_view{ptr, std::end(word)}};
			}
		}

		return std::pair{std::string_view{}, std::string_view{}};
	}

	template<class LetterGroupSet>
	std::pair<std::string_view, std::string_view>
	next_group_longest(std::string_view word, LetterGroupSet const& letter_groups)
	{
		auto ptr = std::end(word);
		while(ptr != std::begin(word))
		{
			auto const slice = std::string_view{std::begin(word), ptr};
			if(auto i = letter_groups.find(slice); i != std::end(letter_groups))
			{
				return std::pair{slice, std::string_view{std::end(slice), std::end(word)}};
			}
			--ptr;
		}

		return std::pair{std::string_view{}, std::string_view{}};
	}
}

#endif