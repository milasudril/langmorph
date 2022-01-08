//@{"dependencies_extra":[{"ref":"stream_tokenizer.o","rel":"implementation"}]}

#ifndef WORDGEN_STREAMTOKENIZER_HPP
#define WORDGEN_STREAMTOKENIZER_HPP

#include <cstdio>
#include <string>

namespace wordgen
{
	class stream_tokenizer
	{
	public:
		explicit stream_tokenizer(FILE* src): m_src{src}
		{ pop(); }

		decltype(auto) top() &
		{ return std::move(m_buffer); }

		std::string_view top() const&
		{
			return m_buffer;
		}

		void pop();

		bool empty() const
		{ return m_src == nullptr && std::size(m_buffer) == 0; }

	private:
		FILE* m_src;
		std::string m_buffer;
	};
}

#endif