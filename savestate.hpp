#ifndef LANGMORPH_SAVESTATE_HPP
#define LANGMORPH_SAVESTATE_HPP

#include "./letter_group_index.hpp"
#include "./word_stats.hpp"
#include "./io_utils.hpp"
#include "./stream_tokenizer.hpp"

namespace langmorph
{
	struct savestate
	{
		letter_group_index letter_groups;
		class word_stats word_stats;
	};

	void store(std::string_view statfile, savestate const& state)
	{
		constexpr auto store_creation_mode = Wad64::FileCreationMode::AllowOverwriteWithTruncation()
			.allowCreation();

		Wad64::FdOwner output_file{std::string{statfile}.c_str(),
			Wad64::IoMode::AllowRead().allowWrite(),
			store_creation_mode};
		Wad64::Archive archive{std::ref(output_file)};
		{
			wad64_output_adapter output{std::ref(archive), "langmorph_data/letter_groups", store_creation_mode};
			auto output_file = create_file(output);
			store(state.letter_groups, output_file.first.get());
		}

		{
			Wad64::OutputFile output{std::ref(archive), "langmorph_data/word_lengths", store_creation_mode};
			store(state.word_stats.length_histogram(), output);
		}

		{
			Wad64::OutputFile output{std::ref(archive), "langmorph_data/transition_rates", store_creation_mode};
			store(state.word_stats.transition_rates(), output);
		}
	}

	auto load(std::type_identity<savestate>, std::string_view statfile)
	{
		constexpr auto load_creation_mode = Wad64::FileCreationMode::DontCare();
		Wad64::FdOwner input_file{std::string{statfile}.c_str(), Wad64::IoMode::AllowRead(), load_creation_mode};
		Wad64::ReadonlyArchive archive{std::ref(input_file)};

		auto letter_groups = with(wad64_input_adapter{std::ref(archive), "langmorph_data/letter_groups"},
			[](auto&& input) {
				auto input_file = create_file(input);
				return load(std::type_identity<letter_group_index>{},
					stream_tokenizer{input_file.first.get()});
			});

		auto word_lengths = load(std::type_identity<histogram>{},
			Wad64::InputFile{Wad64::ArchiveView{archive}, "langmorph_data/word_lengths"});

		auto transition_rates = load(std::type_identity<transition_rate_table>{},
			Wad64::InputFile{Wad64::ArchiveView{archive}, "langmorph_data/transition_rates"});

		if(std::size(letter_groups) != transition_rates.node_count())
		{
			throw std::runtime_error{"Tried to load an invalid stat file"};
		}

		return savestate{std::move(letter_groups), word_stats{std::move(word_lengths), std::move(transition_rates)}};
	}
}
#endif