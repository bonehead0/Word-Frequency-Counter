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
		{
			ch = static_cast<char>(std::tolower(ch));
		}
	}
}

void HandleWord(std::string& Word)
{
	if (IsWordLowercase(Word))
	{
		MakeWordLowercase(Word);
	}

	auto [Begin, End] = std::ranges::remove_if(Word, [](const char ch) -> bool
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

std::string CollectRemainingArgs(int argc, char** argv)
{
	if (!argc || argv == nullptr)
		return {};

	std::string AllArgs;
	AllArgs.reserve(static_cast<size_t>(argc * 10));

	for (int i{}; i < argc; ++i)
	{
		AllArgs.append(argv[i]);
		AllArgs.push_back(' ');
	}

	return AllArgs;
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
		"-p \t\t\t(Flag to enter paragraph)\n"
		"-h \t\t\t(Print Usage or help)\n"
		"---------------------------------------------------\n";

	std::cout << UsageString;
}

void ListOccurences(const std::unordered_map<std::string, uint32_t>& Words)
{
	int MaxWordLength = 0;
	for (const auto& [word, occurrence] : Words) {
		MaxWordLength = std::max(MaxWordLength, static_cast<int>(word.length()));
	}
	const int WordWidth = MaxWordLength + 4; // Add padding

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
		throw std::invalid_argument("File doesn't exist: " + Filename.string());

	std::ifstream File(Filename);
	if (!File.is_open())
		throw std::invalid_argument("Unable to open file: " + Filename.string());

	std::ostringstream oss;
	oss << File.rdbuf();

	HandleParagraph(oss.str());
}

int Execute(int& argc, char**& argv)
{
	std::string ProgramName = ShiftCommandLineArgs(argc, argv);
	try
	{
		if (argc < 1)
		{
			PrintUsage(ProgramName);
			std::exit(0);
		}

		std::string Argument = ShiftCommandLineArgs(argc, argv);

		if (Argument[0] == '-')
		{
			if (Argument.at(1) == 'p')
			{
				if (argc > 0)
				{
					std::string Paragraph;
					Paragraph = CollectRemainingArgs(argc, argv);
					HandleParagraph(Paragraph);
				}
				else
				{
					throw std::invalid_argument("no argument passed after the flag, expected paragraph");
				}
			}
			else if (Argument[0] == 'h')
			{
				PrintUsage(ProgramName);
			}
			else
			{
				throw std::invalid_argument("invalid flag");
			}
		}
		else
		{
			//if not flag, then it's a file
			HandleFile(Argument);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr
			<< "---------------------------------------\n"
			<< "[!Program Execution Failed!]\nReason: "
			<< e.what() << '\n'
			<< "Program Name: " << ProgramName << '\n'
			<< "---------------------------------------\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}