//@	{"dependencies_extra":[{"ref":"./savestate.o", "rel":"implementation"}]}

#ifndef LANGMORPH_SAVESTATE_HPP
#define LANGMORPH_SAVESTATE_HPP

#include "./letter_group_index.hpp"
#include "./word_stats.hpp"

namespace langmorph
{
	struct savestate
	{
		letter_group_index letter_groups;
		class word_stats word_stats;
	};

	void store(
		std::string_view statefile,
		savestate const& state,
		std::string_view statefile_entry
	);

	savestate load(
		std::type_identity<savestate>,
		std::string_view statefile,
		std::string_view statefile_entry
	);

	savestate remove_unused_letter_groups(savestate&& state);
}
#endif