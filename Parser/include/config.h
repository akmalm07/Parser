#pragma once

#include <iostream>

#include <vector>
#include <array>
#include <cstdint>
#include <string_view>
#include <string>



namespace parser {


	using TockenizedSection = std::vector<std::string_view>; // A section of the file that has been tokenized, each token is a string_view to the tokenized version of the entire file.

	using TockenizedSections = std::vector <TockenizedSection>; // A vector of sections, each section is a vector of string_views that point to the tokenized version of the entire file.
	
	using TockenizedSectionsIterator = std::vector <TockenizedSection>::iterator; // Iterator value for the TockenizedSections, used to iterate over the sections of the file that have been tokenized.
	
	using TockenizedFile = std::vector<std::string>; // A vector of strings, each string is a section of the file that has been tokenized. Each string is a tockenized version of the entire file, where each token is separated by whitespace or newlines.

	using UntockeizedFile = std::string; // String that contains the entire file that has not been tockenized yet.


	enum class ParserReadType : uint32_t
	{
		Binary,
		Text
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