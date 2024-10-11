/*
* //(WFC) Word Frequency Counter
* //Author: Bonehead <[-_-]>
* //Dated: 06-10-24
*/
#pragma once

#include<string>
#include<filesystem>
#include<unordered_map>

bool IsWordLowercase(std::string_view Word);
void MakeWordLowercase(std::string& Word);
void HandleWord(std::string& Word);
std::string ShiftCommandLineArgs(int& argc, char**& argv);
std::optional<std::string> CollectRemainingArgs(int argc, char** argv);
void PrintUsage(std::string_view ProgramName);
void ListOccurences(const std::unordered_map<std::string, uint32_t>& Words);
void HandleParagraph(std::string_view Paragraph);
void HandleFile(const std::filesystem::path& Filename);
void HandleMultipleFiles(std::string_view RemainingArgs);
int Execute(int& argc, char**& argv);