/*
* //(WFC) Word Frequency Counter
* //Author: Bonehead <[-_-]>
* //Dated: 06-10-24
*/
#include"WFC.hpp"
#include<fstream>
#include<sstream>
#include<iostream>
#include<algorithm>

static std::string ProgramName;

static void ErrorApproach(std::string_view Message, bool Exit = true)
{
	std::cerr << Message << '\n';
	if (Exit)
	{
		std::cerr << "Program Exited!!!\n";
		std::exit(EXIT_FAILURE);
	}
}

bool IsWordLowercase(std::string_view Word)
{
	for (const char ch : Word)
	{
		if (std::isalpha(ch) && !std::islower(ch))
			return false;
	}
	return true;
}

void MakeWordLowercase(std::string& Word)
{
	for (char& ch : Word)
	{
		if (std::isalpha(ch) && !std::islower(ch))
			ch = static_cast<char>(std::tolower(ch));
	}
}

void HandleWord(std::string& Word)
{
	if (IsWordLowercase(Word))
	{
		MakeWordLowercase(Word);
	}

	auto [Begin, End] = std::ranges::remove_if(Word,
		[](const char ch) -> bool
		{
			if (std::isgraph(ch) && !std::isalnum(ch))
				return true;
			return false;
		});

	Word.erase(Begin, End);
}

std::string ShiftCommandLineArgs(int& argc, char**& argv)
{
	std::string Arg = *argv++;
	--argc;
	return Arg;
}

std::optional<std::string> CollectRemainingArgs(int argc, char** argv)
{
	if (!argc || argv == nullptr)
		return std::nullopt;

	std::string AllArgs;
	AllArgs.reserve(static_cast<size_t>(argc * 7));

	for (int i{}; i < argc; ++i)
	{
		AllArgs.append(argv[i]);
		AllArgs.push_back(' ');
	}

	AllArgs.shrink_to_fit();
	return AllArgs;
}

void HandleFlags(char Flag, int argc, char** argv)
{
	switch (Flag)
	{
	case 'p':
	{
		auto Paragraph = CollectRemainingArgs(argc, argv);
		if (Paragraph)
			HandleParagraph(*Paragraph);
		else
			ErrorApproach("no argument passed after the flag, expected paragraph");
		break;
	}
	case 'h':
		PrintUsage(ProgramName);
		break;
	case 'f':
	{
		auto RemainingArgs = CollectRemainingArgs(argc, argv);
		if (RemainingArgs)
			HandleMultipleFiles(*RemainingArgs);
		else
			ErrorApproach("No arguments passed after"
				" flag -f\nExpected: filename(s)\n");
		break;
	}
	default:
		ErrorApproach("invalid flag");
	}
}

void PrintUsage(std::string_view ProgramName)
{
	using namespace std::string_literals;

	static const std::string UsageString =
		"---------------------------------------------------\n"
		"Usage: "s + ProgramName.data() + " [OPTION] [INPUT]\n"
		"Input:\n"
		"FILE\t\t\tPath to input File\n"
		"PARAGRAPH\t\tInput Paragraph\n"
		"Option:\n"
		"-p \t\t\t(Paragraph as argument)\n"
		"-h \t\t\t(Print Usage or help)\n"
		"-f \t\t\t(Multiple Files)\n"
		"---------------------------------------------------\n";

	std::cout << UsageString;
}

void ListOccurences(const std::unordered_map<std::string, uint32_t>& Words)
{
	int MaxWordLength = 0;
	for (const auto& [word, occurrence] : Words) {
		MaxWordLength = std::max(MaxWordLength, static_cast<int>(word.length()));
	}
	const int WordWidth = MaxWordLength + 4; // Max Word Length + Added padding

	std::cout
		<< std::left << std::setw(WordWidth)
		<< "Word" << "\t|\t" << "Occurences" << '\n'
		<< "--------------------------------------------\n\n";

	for (const auto& [word, occurence] : Words)
	{
		std::cout
			<< std::setw(WordWidth) << word
			<< "\t|\t"
			<< std::setw(2) << occurence << '\n';
	}
	std::cout << "\n\n";
}

void HandleParagraph(std::string_view Paragraph)
{
	std::unordered_map<std::string, uint32_t> Words;
	std::istringstream ss{ Paragraph.data() };
	std::string Word;

	while (std::getline(ss, Word, ' '))
	{
		HandleWord(Word);
		if (!Words.empty())
		{
			if (auto Iter = Words.find(Word); Iter != Words.end())
			{
				++Iter->second;
				continue;
			}
		}
		Words.insert({ Word, 1 });

	}
	ListOccurences(Words);
}

void HandleFile(const std::filesystem::path& Filename)
{
	if (!exists(Filename))
		ErrorApproach("File doesn't exist: " + Filename.string());

	std::ifstream File(Filename);
	if (!File.is_open())
		ErrorApproach("Unable to open file: " + Filename.string());

	std::ostringstream oss;
	oss << File.rdbuf();

	HandleParagraph(oss.str());
}

void HandleMultipleFiles(std::string_view RemainingArgs)
{
	std::istringstream iss{ RemainingArgs.data() };
	std::string Filename;

	while (std::getline(iss, Filename, ' '))
	{
		std::cout << "Filename: " << Filename << '\n';
		HandleFile(Filename);
	}
}

int Execute(int& argc, char**& argv)
{
	ProgramName = ShiftCommandLineArgs(argc, argv);

	if (argc < 1)
	{
		PrintUsage(ProgramName);
		std::exit(0);
	}

	std::string Argument = ShiftCommandLineArgs(argc, argv);

	if (Argument[0] == '-')
	{
		HandleFlags(Argument.at(1), argc, argv);
	}
	else
	{
		//if not flag, then it's a file
		HandleFile(Argument);
	}

	return EXIT_SUCCESS;
}