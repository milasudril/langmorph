#ifndef LANGMORPH_IOUTILS_HPP
#define LANGMORPH_IOUTILS_HPP

#include <cstdio>
#include <memory>
#include <filesystem>
#include <cerrno>
#include <cstring>

namespace langmorph
{
	namespace fs = std::filesystem;

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
	auto create_file(char const* filename, Args&&... args)
	{
		return std::pair{file_handle{fopen(filename, std::forward<Args>(args)...)}, errno};
	}

	template<class T, class ... Args>
	auto load(fs::path&& filename, T&& loader, Args... args)
	{
		auto res = langmorph::create_file(filename.c_str(), "rb");
		if(res.first == nullptr)
		{
			auto errmsg = strerror(res.second);
			throw std::runtime_error{std::string{"Failed to open "}.append(filename)
				.append(":")
				.append(errmsg)
			};
		}

		return loader(res.first.get(), std::forward<Args>(args)...);
	}

}

#endif
