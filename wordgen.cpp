//@	{"target":{"name":"wordgen.o"}}

#include <random>
#include <string>
#include <map>
#include <set>

using CodeSequence = std::string;

auto single_wovels()
{
	return std::set<CodeSequence>{"a", "e", "i", "o", "u", "y", "ä", "å", "ö"};
}

int main()
{
}