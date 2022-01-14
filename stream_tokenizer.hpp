//@{"dependencies_extra":[{"ref":"stream_tokenizer.o","rel":"implementation"}]}

#ifndef LANGMORPH_STREAMTOKENIZER_HPP
#define LANGMORPH_STREAMTOKENIZER_HPP

#include <cstdio>
#include <string>

namespace langmorph
{
	class stream_tokenizer
	{
	public:
		explicit stream_tokenizer(FILE* src): m_src{src}
		{ pop(); }

		decltype(auto) front() &
		{
			return std::move(m_buffer);
		}

		std::string_view front() const&
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