#ifndef LANGMORPH_LETTERGROUPFILERESOLVER_HPP
#define LANGMORPH_LETTERGROUPFILERESOLVER_HPP

#include <filesystem>
#include <string>

namespace langmorph
{
	namespace fs = std::filesystem;

	class letter_group_file_resolver
	{
	public:
		explicit letter_group_file_resolver(fs::path&& filename):m_filename{std::move(filename)}
		{
		}

		bool fetch_next()
		{
			if(m_filename.empty())
			{ return false; }

			if(m_filename.is_absolute() || m_filename.string().front() == '.')
			{
				m_current_path = std::move(m_filename);
				return true;
			}

			if(auto langmorph_home = getenv("LANGMORPH_HOME"); langmorph_home != nullptr)
			{
				m_current_path = fs::path{langmorph_home} / std::move(m_filename);
				return true;
			}

			m_current_path = read_symlink(fs::path{"/proc/self/exe"}).parent_path()
			               / "share" / "langmorph"
			               / std::move(m_filename);
			return true;
		}

		fs::path const& current() const
		{
			return m_current_path;
		}

	private:
		fs::path m_filename;
		fs::path m_current_path;
	};

	std::string to_string(letter_group_file_resolver const& res)
	{
		return std::string{" "}.append(res.current());
	}
}

#endif