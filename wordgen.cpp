//@	{"target":{"name":"wordgen.o"}}

#include "./io_utils.hpp"
#include "./letter_group.hpp"
#include "./stream_tokenizer.hpp"
#include "./letter_group_index.hpp"

template<class T, class ... Args>
auto load(char const* filename, T&& loader, Args... args)
{
	auto input_file = wordgen::create_file(filename, "rb");
	return loader(input_file.get(), std::forward<Args>(args)...);
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		puts("Too few arguments");
		return -1;
	}

	auto letter_groups = load(argv[1], [](auto file) {
		return load(std::type_identity<wordgen::letter_group_index>{}, wordgen::stream_tokenizer{file});
	});

	return 0;
}