//@	{"target":{"name":"langmorph.o", "pkgconfig_libs":["wad64"]}}

#include "./io_utils.hpp"
#include "./letter_group.hpp"
#include "./stream_tokenizer.hpp"
#include "./letter_group_index.hpp"
#include "./word_stats.hpp"
#include "./prob_distributions.hpp"
#include "./letter_group_file_resolver.hpp"

#include <wad64/archive.hpp>
#include <wad64/readonly_archive.hpp>
#include <wad64/fd_owner.hpp>
#include <wad64/output_file.hpp>
#include <wad64/input_file.hpp>

#include <random>

int show_help_general()
{
	puts(R"(Usage: langmorph <action> [command arguments]

Supported actions:

collect-stats  collects statistics given a file with words and a file with valid letter groups
make-words     generates a list of new words
)");
	return 0;
}

int show_help_collect_stats()
{
	puts(R"(Usage: langmorph collect-stats <stat file> [letter group file] -- <source> [sources...]

# The stat file

This file is used to store statistics. If a letter group file is specified, the file will be
stat file will be recreated, otherwise statistics will be accumulated.

# The Letter group file

The letter group file is a list of valid letter groups. If not specified, the letter groups will be
read from <stat file>. Langmorph uses the following rules to locate the letter group file:

1. If the path is absoulte, that path will be used
2. If the name begins with `.`, the path is expected to be relative to the current
3. The directory specified by the LANGMORPH_HOME environment variable
4. <location of langmorph>/../../share/langmorph

On a UNIX system, langmorph would typically be located in /usr/bin

# Source files

Source files are used to collect statistics. Each word in a source file is tokenized based on the
largest match in the lettergroup file. If the letter group file contains a, c, e, g, h, l, r, s,
and sch, the word schlager, will be interpreted as sch-l-a-g-e-r. Statistics are collected for the
number of tokens in different words, and token succuessors. In this case, there would be one
incidence of an l that follows sch.
)");
	return 0;
}

int show_help_make_words()
{
	return 0;
}

int show_help(std::string_view cmd)
{
	if(cmd == "collect-stats")
	{
		return show_help_collect_stats();
	}

	if(cmd == "make-words")
	{
		return show_help_make_words();
	}
	return show_help_general();
}

int show_help(std::span<std::string_view const> args)
{
	if(std::size(args) == 0)
	{
		return show_help_general();
	}
	return show_help(args[0]);
}

class wad64_stdio_output
{
public:
	static constexpr auto is_output = true;
	static constexpr auto seek_set = Wad64::SeekMode::Set;
	static constexpr auto seek_cur = Wad64::SeekMode::Cur;
	static constexpr auto seek_end = Wad64::SeekMode::End;

	template<class First, class ...Args>
	explicit wad64_stdio_output(First&& first, Args&& ... args)
		requires (!std::same_as<std::decay_t<First>, wad64_stdio_output>):
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

class wad64_stdio_input
{
public:
	static constexpr auto is_output = false;
	static constexpr auto seek_set = Wad64::SeekMode::Set;
	static constexpr auto seek_cur = Wad64::SeekMode::Cur;
	static constexpr auto seek_end = Wad64::SeekMode::End;

	template<class First, class ...Args>
	explicit wad64_stdio_input(First&& first, Args&& ... args)
		requires (!std::same_as<std::decay_t<First>, wad64_stdio_input>):
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

void store(std::string_view statfile,
           langmorph::letter_group_index const& letter_groups,
           langmorph::word_stats const& word_stats)
{
	constexpr auto store_creation_mode = Wad64::FileCreationMode::AllowOverwriteWithTruncation()
		.allowCreation();

	Wad64::FdOwner output_file{std::string{statfile}.c_str(),
		Wad64::IoMode::AllowRead().allowWrite(),
		store_creation_mode};
	Wad64::Archive archive{std::ref(output_file)};
	{
		wad64_stdio_output output{std::ref(archive), "langmorph_data/letter_groups", store_creation_mode};
		auto output_file = langmorph::create_file(output);
		store(letter_groups, output_file.first.get());
	}

	{
		Wad64::OutputFile output{std::ref(archive), "langmorph_data/word_lengths", store_creation_mode};
		store(word_stats.length_histogram(), output);
	}

	{
		Wad64::OutputFile output{std::ref(archive), "langmorph_data/transition_rates", store_creation_mode};
		store(word_stats.transition_rates(), output);
	}
}

void collect_stats(std::string_view statfile,
                   std::span<std::string_view const> sources,
                   std::string_view letter_groups_file)
{
	auto letter_groups = load(langmorph::letter_group_file_resolver{letter_groups_file}, [](auto file) {
		return load(std::type_identity<langmorph::letter_group_index>{}, langmorph::stream_tokenizer{file});
	});

	langmorph::word_stats word_stats{std::size(letter_groups)};
	std::ranges::for_each(sources, [&word_stats, &letter_groups](auto file) {
		word_stats += langmorph::load(file, [&letter_groups](auto file) {
			return load(std::type_identity<langmorph::word_stats>{}, langmorph::stream_tokenizer{file}, letter_groups);
		});
	});

	store(statfile, letter_groups, word_stats);
}

auto load(std::type_identity<langmorph::word_stats>, std::string_view statfile)
{
	constexpr auto load_creation_mode = Wad64::FileCreationMode::DontCare();
	Wad64::FdOwner input_file{std::string{statfile}.c_str(), Wad64::IoMode::AllowRead(), load_creation_mode};
	Wad64::ReadonlyArchive archive{std::ref(input_file)};

	auto letter_groups = langmorph::with(wad64_stdio_input{std::ref(archive), "langmorph_data/letter_groups"},
		[](auto&& input) {
			auto input_file = langmorph::create_file(input);
			return load(std::type_identity<langmorph::letter_group_index>{},
				langmorph::stream_tokenizer{input_file.first.get()});
		});

	auto word_lengths = load(std::type_identity<langmorph::histogram>{},
		Wad64::InputFile{Wad64::ArchiveView{archive}, "langmorph_data/word_lengths"});

	auto transition_rates = load(std::type_identity<langmorph::transition_rate_table>{},
		Wad64::InputFile{Wad64::ArchiveView{archive}, "langmorph_data/transition_rates"});

	if(std::size(letter_groups) != transition_rates.node_count())
	{
		throw std::runtime_error{"Tried to load an invalid stat file"};
	}
}

void collect_stats(std::string_view statfile, std::span<std::string_view const> sources)
{
	load(std::type_identity<langmorph::word_stats>{}, statfile);
	printf("statfile: %s\n", std::data(statfile));
	printf("sources:\n");
	std::ranges::for_each(sources, [](auto item){
		printf("    %s\n", std::data(item));
	});
}

int collect_stats(std::span<std::string_view const> args)
{
	if(std::size(args) < 3)
	{
		puts(R"(Try langmorph help collect-stats)");
		return -1;
	}

	auto const has_letter_group_file = args[1] != "--";
	if(has_letter_group_file && std::size(args) < 4)
	{
		puts(R"(Try langmorph help collect-stats)");
		return -1;
	}

	auto const statfile = args[0];
	auto const letter_group_file = has_letter_group_file ? args[1] : std::string_view{};
	auto const sources = std::span{std::begin(args) + (has_letter_group_file? 3 : 2), std::end(args)};

	if(has_letter_group_file)
	{
		collect_stats(statfile, sources, letter_group_file);
	}
	else
	{
		collect_stats(statfile, sources);
	}

	return 0;
}

int make_words(std::span<std::string_view const>)
{
	puts("Make words");
	return 0;
}

std::vector<std::string_view> get_action_args(int argc, char const* const* argv)
{
	std::vector<std::string_view> ret;
	std::transform(argv + std::min(argc, 2), argv + argc, std::back_inserter(ret), [](auto item){
		return std::string_view{item};
	});
	return ret;
}

std::string_view get_action(int argc, char const* const* argv)
{
	if(argc < 2)
	{
		return std::string_view{"help"};
	}

	return std::string_view{argv[1]};
}

int main(int argc, char** argv)
try
{
	auto action = get_action(argc, argv);
	auto action_args = get_action_args(argc, argv);
	if(action == "help")
	{
		return show_help(action_args);
	}

	if(action == "collect-stats")
	{
		return collect_stats(action_args);
	}

	if(action == "make-words")
	{
		return make_words(action_args);
	}

	return show_help(action_args);
#if 0

	auto letter_groups = langmorph::load(argv[1], [](auto file) {
		return load(std::type_identity<langmorph::letter_group_index>{}, langmorph::stream_tokenizer{file});
	});

	auto word_stats = langmorph::load(argv[2], [&letter_groups](auto file) {
		return load(std::type_identity<langmorph::word_stats>{}, langmorph::stream_tokenizer{file}, letter_groups);
	});

	auto word_length = langmorph::gen_pmf(word_stats.length_histogram()());
	langmorph::bivar_discrete_distribution letter_group_probs{word_stats.transition_rates()};

	std::mt19937 rng;

	for(size_t k = 0; k != 1024; ++k)
	{
		auto length = word_length(rng);
		auto current_group = letter_group_probs.col(0, rng);
		std::string word{letter_groups.get(langmorph::letter_group_id{current_group}).value()};
		assert(length != 0);
		for(size_t l = 1; l != length; ++l)
		{
			current_group = letter_group_probs.col(current_group, rng);
			word += letter_groups.get(langmorph::letter_group_id{current_group}).value();
		}
		puts(word.c_str());
	}

	return 0;
#endif
}
catch(std::exception const& e)
{
	fprintf(stderr, "%s\n", e.what());
	return -1;
}