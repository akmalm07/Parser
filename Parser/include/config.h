#pragma once

#include <iostream>

#include <vector>
#include <array>
#include <cstdint>
#include <string_view>
#include <string>
#include <memory>


namespace parser {


	using TockenizedUnsectionedFile = std::vector<std::string_view>; // A vector of string_views, each string_view is a token in the file that has been tockenized. Each token is a view into the original file, so it can be used to access the original file without copying it. This is useful for large files where copying the entire file would be expensive in terms of memory and performance.
	
	using TockenizedUnsectionedFileIterator = TockenizedUnsectionedFile::iterator; // A vector of string_views, each string_view is a token in the file that has been tockenized. Each token is a view into the original file, so it can be used to access the original file without copying it. This is useful for large files where copying the entire file would be expensive in terms of memory and performance.
	using TockenizedUnsectionedFileIteratorConst = TockenizedUnsectionedFile::const_iterator; // A vector of string_views, each string_view is a token in the file that has been tockenized. Each token is a view into the original file, so it can be used to access the original file without copying it. This is useful for large files where copying the entire file would be expensive in terms of memory and performance.

	using TokenizedSection = std::vector<std::string_view>; // A section of the file that has been tokenized, each token is a string_view to the tokenized version of the entire file. This is useful for sections of the file that have been tokenized, where each token is a view into the original file, so it can be used to access the original file without copying it. This is useful for large files where copying the entire file would be expensive in terms of memory and performance.
	using TokenizedSections = std::vector <std::vector<std::string_view>>; // A section of the file that has been tokenized, each token is a string_view to the tokenized version of the entire file. This is useful for sections of the file that have been tokenized, where each token is a view into the original file, so it can be used to access the original file without copying it. This is useful for large files where copying the entire file would be expensive in terms of memory and performance.


	/*
	using TockenizedSection = std::vector<std::string_view>; // A section of the file that has been tokenized, each token is a string_view to the tokenized version of the entire file.

	using TockenizedSections = std::vector <TockenizedSection>; // A vector of sections, each section is a vector of string_views that point to the tokenized version of the entire file.
	
	using TockenizedSectionsIterator = TockenizedSections::iterator; // Iterator value for the TockenizedSections, used to iterate over the sections of the file that have been tokenized.
	*/

	using EntireTokenizedFile = std::vector<std::string_view>; // A vector of strings, each string is a section of the file that has been tokenized. Each string is a tockenized version of the entire file, where each token is separated by whitespace or newlines.

	using EntireUntokenizedFile = std::string; // String that contains the entire file that has not been tockenized yet.


	enum class ParserReadType : uint32_t
	{
		Binary,
		Text
	};

	enum class SmartPtrType : uint32_t
	{
		None = 0,
		Shared,
		Unique,
		Weak
	};

}

std::string green_text(std::string_view text);


std::string red_text(std::string_view text);


std::string yellow_text(std::string_view text);


#define PARSER_LOG_ERR \
	red_text("[Parser Error]") << ": "

#define PARSER_LOG_WARN \
	yellow_text("[Parser Warning]") << ": "

#define PARSER_LOG_INFO \
	green_text("[Parser Info]") << ": "