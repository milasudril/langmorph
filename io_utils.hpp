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
	concept path_resolver = requires(T a)
	{
		{a.current().c_str()} -> std::same_as<char const*>;
		{a.fetch_next()} -> std::same_as<bool>;
		{to_string(a)} -> std::convertible_to<std::string>;
	};

	template<path_resolver Resolver, class Loader, class ... Args>
	auto load(Resolver&& resolver, Loader&& loader, Args&&... args)
	{
		auto last_error = errno;
		while(resolver.fetch_next())
		{
			auto f = create_file(resolver.current().c_str(), "rb");
			if(f.first != nullptr)
			{
				return loader(f.first.get(), std::forward<Args>(args)...);
			}
			last_error = f.second;
		}

		auto errmsg = strerror(last_error);
		throw std::runtime_error{std::string{"Failed to open one of"}.append(to_string(resolver))
			.append(":")
			.append(errmsg)
		};
	}
}

#endif
