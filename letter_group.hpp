#ifndef LANGMORPH_LETTERGROUP_HPP
#define LANGMORPH_LETTERGROUP_HPP

#include <string>
#include <type_traits>
#include <vector>

namespace langmorph
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
			if(letter_groups.contains(slice))
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
			if(letter_groups.contains(slice))
			{
				return std::pair{slice, std::string_view{std::end(slice), std::end(word)}};
			}
			--ptr;
		}

		return std::pair{std::string_view{}, std::string_view{}};
	}

	template<class LetterGroupSet>
	std::vector<std::string_view> split_longest(std::string_view word, LetterGroupSet const& letter_groups)
	{
		std::vector<std::string_view> ret;
		auto token = next_group_longest(word, letter_groups);
		while(std::size(token.second) != 0)
		{
			ret.push_back(token.first);
			token = next_group_longest(token.second, letter_groups);
		}

		if(std::end(token.second) == std::end(word))
		{
			ret.push_back(token.first);
		}
		else
		{
			ret.clear();
		}

		return ret;
	}

	template<class LetterGroupSet, class TokenStream>
	LetterGroupSet load(std::type_identity<LetterGroupSet>, TokenStream&& tokens)
	{
		LetterGroupSet ret;
		while(!tokens.empty())
		{
			ret.insert(letter_group{std::move(tokens.front())});
			tokens.pop();
		}
		return ret;
	}
}

#endif