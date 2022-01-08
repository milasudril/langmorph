#ifndef WORDGEN_IOUTILS_HPP
#define WORDGEN_IOUTILS_HPP

#include <cstdio>
#include <memory>

namespace wordgen
{
	struct file_deleter
	{
		void operator()(FILE* f)
		{
			if(f != nullptr)
			{ fclose(f); }
		}
	};

	using file_handle = std::unique_ptr<FILE, file_deleter>;

	template<class... Args>
	file_handle create_file(Args&&... args)
	{
		return file_handle{fopen(std::forward<Args>(args)...)};
	}
}

#endif
