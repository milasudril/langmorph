#ifndef WORDGEN_LETTERGROUP_HPP
#define WORDGEN_LETTERGROUP_HPP

#include <string>

namespace wordgen
{
	class letter_group
	{
	public:
		explicit letter_group(std::string&& str):m_chars{str}{}

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
	std::pair<std::string_view, letter_group const*>
	next_group(std::string_view word, LetterGroupSet const& letter_groups)
	{
		auto ptr = std::begin(word);
		while(ptr != std::end(word))
		{
			if(auto i = letter_groups.find(std::string_view{ptr, std::end(word)}); i != std::end(letter_groups))
			{
				return std::pair{std::string_view{std::begin(word), ptr + 1}, i};
			}
		}

		return std::pair{std::string_view{}, nullptr};
	}
}


#endif