//@{"target":{"name":"stream_tokenizer.o"}}

#include "./stream_tokenizer.hpp"

namespace
{
	constexpr bool is_whitespace(int ch_in)
	{
		return static_cast<char>(ch_in) >= '\0' && static_cast<char>(ch_in) <= ' ';
	}

	constexpr bool is_not_alnum(int ch_in)
	{
		return (static_cast<char>(ch_in) >= '\0' && static_cast<char>(ch_in) < '0')
			|| (static_cast<char>(ch_in) >= ':' && static_cast<char>(ch_in) < 'A')
			|| (static_cast<char>(ch_in) >= '[' && static_cast<char>(ch_in) < 'a')
			|| (static_cast<char>(ch_in) >= '{' && static_cast<char>(ch_in) <= 0x7f);
	}

	int eat_whitespace(FILE* src)
	{
		while(true)
		{
			auto ch_in = getc(src);
			if(ch_in == EOF)
			{ return ch_in; }

			if(!is_whitespace(ch_in))
			{ return ch_in; }
		}
	}

}

void wordgen::stream_tokenizer::pop()
{
	if(m_src == nullptr)
	{ return; }
	m_buffer.clear();
	auto ch_in = eat_whitespace(m_src);

	auto last_delim = std::string::npos;
	enum class state:int{normal, delimiter};
	auto current_state = state::normal;
	while(true)
	{
		if(ch_in == EOF)
		{
			m_src = nullptr;
			return;
		}

		if(static_cast<char>(ch_in) >= '\0' && ch_in <= static_cast<char>(' '))
		{
			if(last_delim != std::string::npos)
			{ m_buffer.resize(last_delim); }
			return;
		}

		switch(current_state)
		{
			case state::normal:
				if(is_not_alnum(ch_in))
				{
					last_delim = std::size(m_buffer);
					m_buffer += static_cast<char>(ch_in);
					current_state = state::delimiter;
				}
				else
				{
					m_buffer += static_cast<char>(ch_in);
				}
				break;

			case state::delimiter:
				if(is_not_alnum(ch_in))
				{
					m_buffer += static_cast<char>(ch_in);
				}
				else
				{
					last_delim = std::string::npos;
					m_buffer += static_cast<char>(ch_in);
					current_state = state::normal;
				}
				break;
		}

		ch_in = getc(m_src);
	}
}