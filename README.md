# langmorph

A command line tool to generate new words, given a set of known letter groups, and a text corpus.

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

## The letter group file

The letter group file is a text file which lists known letter groups. A letter group is a cluster of letters that should be treated as an entity. Within the latin script this would be all letters, but also some combination of letters, depending on language. For English, `ch`, `sh`, `ght`, `ng`, `th`, and `wh`, are examples of common multi-letter groups. A letter group can also contain punctuations, such as `'d`.

# The text corpus

The text corpus should match the contents of the letter group file. In case a word within the corpus contains at least one unrecognized letter group, it is ignored.