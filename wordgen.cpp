//@	{"target":{"name":"wordgen.o"}}

#include <random>
#include <string>
#include <map>
#include <set>

using CodeSequence = std::string;

auto single_wovels()
{
	return std::set<CodeSequence>{"a", "e", "i", "o", "u", "y", "ä", "å", "é", "ö", "ü"};
}

auto single_consonants()
{
	return std::set<CodeSequence>{"b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n", "p", "q", "r", "s", "t", "v", "w", "x", "z", "þ" ,"ð"};
}

int main()
{
}