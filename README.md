# Introduction

`langmorph` is a command line utility and library that generates new words, given a set of known letter groups, and a text corpus.

## Usecases

The intent of `langmorph` is to generate new words that would fit naturally within an existing language. As such, it can be used to generate placeholder names, or even text "Lorem Ipsum", in a language of choice. It is also useful for measuring performance on tables which uses strings as a key. It can also be used to generate names for use in fiction.

## Some generated words

The list below shows some words produced by feeding `langmorph` with statistics from Shakespeares plays.

|                 |                   |                 |                |
| :---            | :---              | :---            | :---           |
| bais            | Thave             | savot           | bupece         |
| when'd          | bese              | bustr           | mer's          |
| boreomphrels    | pejettl           | nethetedit      | feild          |
| wier            | harer             | hensp           | trumpt         |
| Buat            | eakim             | thivey          | leathe         |
| ulis            | Thelouryourse     | foble           | fonaregay      |
| mereve          | Wiompere          | papave          | cohistyous     |
| Bute            | wele              | herse           | Eisy           |
| helive          | ford's            | dapealor        | hare           |
| livene          | rave              | hark'd          | Midye          |
| fealy           | meller            | Burtus          | slelty         |
| heleie          | Jome              | thereft         | cedin          |
| Tisey           | withe             | gooke           | porere         |
| notly           | fiveth            | bueor           | plaportullord  |
| lilst           | Enfinoy           | hathat          | dithe          |
| fimat           | erer              | wicas           | presay         |
| shathits        | acker             | ashoss'd        | furew          |
| Buty            | Bustr             | demast          | hacke          |
| woured's        | dreakimim         | Chrike          | couldillor     |
| Sake            | vents             | shertepomis     | Thicove        |
| shordidy        | Suicheep          | beavefard       | fonemour       |
| divell          | ulips             | thety           | addendet       |
| grone           | time              | Wisut           | terlanksis     |
| ganno           | fowind            | matand          | frivoung       |
| Clave           | more              | gake            | thandable      |

# Using langmorph

The command `langmorph` is used as follows

`langmorph` **action** *command arguments*

Supported actions:

| Name                          | Description                                                                     |
| :---                          | :---                                                                            |
| `collect-stats`               | collects statistics given a file with words and a file with valid letter groups |
| `help`                        | Shows help about `langmorph` or a given **action**                              |
| `make-words`                  | generates a list of new words                                                   |
| `remove-unused-letter-groups` | removes unused letter groups (those with 0 occurrences) from a stat file        |

## The letter group file

The letter group file is a text file which lists known letter groups. A letter group is a cluster of letters that should be treated as an entity. Within the latin script this would be all letters, but also some combination of letters, depending on language. For English, `ch`, `sh`, `ght`, `ng`, `th`, and `wh`, are examples of common multi-letter groups. A letter group can also contain punctuations, such as `'d`.

## The text corpus

The text corpus should match the contents of the letter group file. In case a word within the corpus contains at least one unrecognized letter group, it is ignored.

## The stat file

The stat file contains all statistics used to generate new words. For simplicity, the data is stored raw binary form in a `wad64` archive. Currently, transferring the file between systems with different byte order will not work.

## How to compile langmorph

1. Install `maike2` and `wad64`, and the standard C++ build utils
2. Run the command `make`

## How to install langmorph

1. Compile langmorph
2. Run the command `sudo make install`. If you do not have `sudo` privileges, you can set the PREFIX environment variable

## Simple library usage

Given the file `demo.cpp`

```c++
#include <langmorph/word_factory.hpp>
#include <cstdio>
#include <random>

int main()
{
	auto factory = create(
		std::type_identity<langmorph::word_factory>{},
		"data/shakespeare.wad64",  // Name of stat file
		"langmorph_data"           // "Lump" of langmorph data are stored in stats.wad64
	);

	std::mt19937 rng;
	for(size_t k = 0; k != 100; ++k)
	{ puts(factory(rng).c_str()); }
}
```

You should be able to compile and link the application by using the following command

```bash
g++ $(pkg-config --cflags langmorph) -std=c++20 -o demo demo.cpp $(pkg-config --libs langmorph)
```