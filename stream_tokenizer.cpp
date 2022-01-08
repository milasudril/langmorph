//@{"target":{"name":"stream_tokenizer.o"}}

#include "./stream_tokenizer.hpp"

namespace
{
	int eat_whitespace(FILE* src)
	{
		while(true)
		{
			auto ch_in = getc(src);
			if(ch_in == EOF)
			{ return ch_in; }

			if(!(static_cast<char>(ch_in) >= '\0' && static_cast<char>(ch_in) <= ' '))
			{ return ch_in; }
		}
	}
}

void wordgen::stream_tokenizer::pop()
{
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
			m_buffer.resize(last_delim);
			return;
		}

		switch(current_state)
		{
			case state::normal:
				if(static_cast<char>(ch_in) >= '0' && static_cast<char>(ch_in) <= 'A')
				{
					last_delim = std::size(m_buffer);
					current_state = state::delimiter;
				}
				else
				{
					m_buffer+=ch_in;
				}
				break;

			case state::delimiter:
				if(!(static_cast<char>(ch_in) >= '0' && static_cast<char>(ch_in) <= 'A'))
				{
					m_buffer+=ch_in;
					current_state = state::normal;
				}
				break;
		}

		ch_in = getc(m_src);
	}
}