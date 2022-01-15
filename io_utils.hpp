#ifndef LANGMORPH_IOUTILS_HPP
#define LANGMORPH_IOUTILS_HPP

#include <cstdio>
#include <memory>
#include <filesystem>
#include <cerrno>
#include <cstring>
#include <ranges>

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

	template<class Loader, class ... Args>
	auto load(fs::path const& filename, Loader&& loader, Args&&... args)
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

	template<class T>
	concept file_list = std::ranges::input_range<T> && requires(T a)
	{
		{a->c_str()} -> std::same_as<char const*>;
	};

	template<file_list FileList, class Loader, class ... Args>
	auto load(FileList&& file_list, Loader&& loader, Args&&... args)
	{
		auto i = std::begin(file_list);
		auto last_error = errno;
		while(i != std::end(file_list))
		{
			auto f = create_file(i->c_str(), "rb");
			if(f.first != nullptr)
			{
				return loader(f.first.get(), std::forward<Args>(args)...);
			}
			last_error = f.second;
			++i;
		}

		auto errmsg = strerror(last_error);
		throw std::runtime_error{std::string{"Failed to open "}.append(to_string(file_list))
			.append(":")
			.append(errmsg)
		};
	}
}

#endif
