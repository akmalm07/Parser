#pragma once

#include <iostream>

#include <vector>
#include <array>
#include <cstdint>
#include <string_view>
#include <string>
#include <memory>
#include <limits>


#define ANONYMUS_SECTIONS_NOT_DEFUALT_CHECKED_BY_RULE 0 // 1 for yes, 0 for no. This is used to determine if the sections that are created by the section handler will be checked by the rules by default. If this is set to 1, then the sections will be checked by the rules by default. If this is set to 0, then the sections will not be checked by the rules by default. This can be changed later on in the code if needed.


namespace parser {


	using TockenizedUnsectionedFile = std::vector<std::string_view>; // A vector of string_views, each string_view is a token in the file that has been tockenized. Each token is a view into the original file, so it can be used to access the original file without copying it. This is useful for large files where copying the entire file would be expensive in terms of memory and performance.
	
	using TockenizedUnsectionedFileIterator = TockenizedUnsectionedFile::iterator; // A vector of string_views, each string_view is a token in the file that has been tockenized. Each token is a view into the original file, so it can be used to access the original file without copying it. This is useful for large files where copying the entire file would be expensive in terms of memory and performance.
	using TockenizedUnsectionedFileIteratorConst = TockenizedUnsectionedFile::const_iterator; // A vector of string_views, each string_view is a token in the file that has been tockenized. Each token is a view into the original file, so it can be used to access the original file without copying it. This is useful for large files where copying the entire file would be expensive in terms of memory and performance.

	using TokenizedSection = std::vector<std::string_view>; // A section of the file that has been tokenized, each token is a string_view to the tokenized version of the entire file. This is useful for sections of the file that have been tokenized, where each token is a view into the original file, so it can be used to access the original file without copying it. This is useful for large files where copying the entire file would be expensive in terms of memory and performance.
	using TokenizedSections = std::vector<std::vector<std::string_view>>; // A section of the file that has been tokenized, each token is a string_view to the tokenized version of the entire file. This is useful for sections of the file that have been tokenized, where each token is a view into the original file, so it can be used to access the original file without copying it. This is useful for large files where copying the entire file would be expensive in terms of memory and performance.
	
	struct SectionCoords
	{
		size_t start;
		size_t end;
		size_t identifier;

		SectionCoords(size_t start = 0, size_t end = 0, size_t identifier = 0) : start(start), end(end), identifier(identifier) {}
	};

	struct TokenizedSectionizedCompact // A compact representation of a tokenized section, which includes the tokens and their coordinates in the original file.
	{
	public:

		TockenizedUnsectionedFile tokens;
		
		std::vector<SectionCoords> coords;

	};

	using EntireTokenizedFile = std::vector<std::string_view>; // A vector of strings, each string is a section of the file that has been tokenized. Each string is a tockenized version of the entire file, where each token is separated by whitespace or newlines.

	using EntireUntokenizedFile = std::string; // String that contains the entire file that has not been tockenized yet.

	/*
	using TockenizedSection = std::vector<std::string_view>; // A section of the file that has been tokenized, each token is a string_view to the tokenized version of the entire file.

	using TockenizedSections = std::vector <TockenizedSection>; // A vector of sections, each section is a vector of string_views that point to the tokenized version of the entire file.
	
	using TockenizedSectionsIterator = TockenizedSections::iterator; // Iterator value for the TockenizedSections, used to iterate over the sections of the file that have been tokenized.
	*/


	template <class T>
	using view_ptr = T const* const;
	

	template <class T>
	using view_ptr_non_const = T const*;



	enum class ParserReadType : uint32_t
	{
		Binary,
		Text
	};

	enum class HasIdentifier : bool
	{
		No = false,
		Yes = true
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