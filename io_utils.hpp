#ifndef LANGMORPH_IOUTILS_HPP
#define LANGMORPH_IOUTILS_HPP

#include <wad64/archive.hpp>
#include <wad64/readonly_archive.hpp>
#include <wad64/fd_owner.hpp>
#include <wad64/output_file.hpp>
#include <wad64/input_file.hpp>

#include <cstdio>
#include <memory>
#include <filesystem>
#include <cerrno>
#include <cstring>
#include <span>

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

	template<class IoHandler>
	int stdio_seek_wrapper(void* cookie, off64_t* offset, int whence)
	{
		if(offset == nullptr){ return -1; }

		auto& obj = *static_cast<IoHandler*>(cookie);
		switch(whence)
		{
			case SEEK_SET:
				*offset = obj.seek(*offset, IoHandler::seek_set);
				return 0;
			case SEEK_CUR:
				*offset = obj.seek(*offset, IoHandler::seek_cur);
				return 0;
			case SEEK_END:
				*offset = obj.seek(*offset, IoHandler::seek_end);
				return 0;
			default: return -1;
		}
	}

	template<class IoHandler>
	int stdio_close_wrapper(void* cookie)
	{
		static_cast<IoHandler*>(cookie)->close();
		return 0;
	}

	template<class... Args>
	auto create_file(char const* filename, Args&&... args)
	{
		auto f = fopen(filename, std::forward<Args>(args)...);
		return std::pair{file_handle{f}, errno};
	}

	template<class IoHandler, class... Args>
	auto create_file(IoHandler& handler)
	{
		if constexpr(IoHandler::is_output)
		{
			return std::pair{file_handle{fopencookie(&handler, "wb", cookie_io_functions_t{
				nullptr,
				[](void* cookie, char const* buff, size_t size) -> ssize_t {
					return static_cast<IoHandler*>(cookie)->write(std::as_bytes(std::span{buff, size}));
				},
				stdio_seek_wrapper<IoHandler>,
				stdio_close_wrapper<IoHandler>
			})}, 0};
		}
		else
		{
			return std::pair{file_handle{fopencookie(&handler, "rb", cookie_io_functions_t{
				[](void* cookie, char* buff, size_t size) -> ssize_t {
					return static_cast<IoHandler*>(cookie)->read(std::as_writable_bytes(std::span{buff, size}));
				},
				nullptr,
				stdio_seek_wrapper<IoHandler>,
				stdio_close_wrapper<IoHandler>
			})}, 0};
		}
	}

	template<class Loader, class ... Args>
	decltype(auto) load(fs::path const& filename, Loader&& loader, Args&&... args)
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
	decltype(auto) load(Resolver&& resolver, Loader&& loader, Args&&... args)
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

	template<class T>
	concept input_handler = requires(T a)
	{
		{a.read(std::declval<std::span<std::byte>>())} -> std::convertible_to<int64_t>;
	};

	template<input_handler Handler, class Loader, class... Args>
	decltype(auto) load(Handler& handler, Loader&& loader, Args&&... args)
	{
		auto res = langmorph::create_file(handler);
		return loader(res.first.get(), std::forward<Args>(args)...);
	}

	template<class Obj, class Function, class ... Args>
	decltype(auto) with(Obj&& obj, Function&& f, Args&&... args)
	{
		return f(std::forward<Obj>(obj), std::forward<Args>(args)...);
	}

	class wad64_output_adapter
	{
	public:
		static constexpr auto is_output = true;
		static constexpr auto seek_set = Wad64::SeekMode::Set;
		static constexpr auto seek_cur = Wad64::SeekMode::Cur;
		static constexpr auto seek_end = Wad64::SeekMode::End;

		template<class First, class ...Args>
		explicit wad64_output_adapter(First&& first, Args&& ... args)
			requires (!std::same_as<std::decay_t<First>, wad64_output_adapter>):
			m_output{std::forward<First>(first), std::forward<Args>(args)...}
		{}

		decltype(auto) write(std::span<std::byte const> buffer)
		{
			return m_output.write(buffer);
		}

		decltype(auto) seek(int64_t offset, Wad64::SeekMode mode)
		{
			return m_output.seek(offset, mode);
		}

		static void close() {}


	private:
		Wad64::OutputFile m_output;
	};

	class wad64_input_adapter
	{
	public:
		static constexpr auto is_output = false;
		static constexpr auto seek_set = Wad64::SeekMode::Set;
		static constexpr auto seek_cur = Wad64::SeekMode::Cur;
		static constexpr auto seek_end = Wad64::SeekMode::End;

		template<class First, class ...Args>
		explicit wad64_input_adapter(First&& first, Args&& ... args)
			requires (!std::same_as<std::decay_t<First>, wad64_input_adapter>):
			m_input{Wad64::ArchiveView{std::forward<First>(first)}, std::forward<Args>(args)...}
		{}

		decltype(auto) read(std::span<std::byte> buffer)
		{
			return m_input.read(buffer);
		}

		decltype(auto) seek(int64_t offset, Wad64::SeekMode mode)
		{
			return m_input.seek(offset, mode);
		}

		static void close() {}


	private:
		Wad64::InputFile m_input;
	};
}

#endif
