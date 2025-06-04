#pragma once
#include <iostream>
#include <string>


#include <fstream>
#include <string_view>
#include <vector>
#include <sstream>
#include <filesystem>
#include <type_traits>
#include <memory>
#include <variant>


namespace parser {


	using TockenizedSection = std::vector<std::string_view>; // A section of the file that has been tokenized, each token is a string_view to the tokenized version of the entire file.

	using TockenizedSections = std::vector <TockenizedSection>; // A vector of sections, each section is a vector of string_views that point to the tokenized version of the entire file.
	
	using TockenizedFile = std::vector <std::string>; // A vector of strings, each string is a section of the file that has been tokenized. Each string is a tockenized version of the entire file, where each token is separated by whitespace or newlines.

	using UntockeizedFile = std::string; // String that contains the entire file that has not been tockenized yet.


	enum class ParserReadType : uint32_t
	{
		Binary,
		Text
	};

	enum class ParserRule : uint32_t
	{
		//Global
		CannotIncludeInFile,
		MustIncludeInFile,

		//Local (Section Oriented)
		CannotInlcude,
		MustInclude,

		//Local (Section Oriented) with two targets
		MustIncludeBefore,
		MustIncludeAfter,
		MustIncludeBetween,
	};

	enum class ParserSectionCreator : uint32_t
	{
		//Creating Local spaces
		NewSectionWhenFound,
		NewSectionWhenBetween,
		NewSectionWhenAfter,
		NewSectionWhenBefore,

	};


	struct Rule
	{
		ParserRule type;
		std::string errMsg;
		virtual ~Rule() = default;
	};


	struct RuleWithOneTarget : Rule
	{

		std::string target;
		RuleWithOneTarget(ParserRule r, std::string_view t, std::string_view e)
		{
			type = r;
			target = t;
			errMsg = e;
		}
	};

	struct RuleWithTwoTarget : Rule
	{
		std::string target1;
		std::string target2;
		RuleWithTwoTarget(ParserRule r, std::string_view t1, std::string_view t2, std::string_view e)
		{
			type = r;
			target1 = t1;
			target2 = t2;
			errMsg = e;
		}
	};

}

std::string green_text(std::string_view text) 
{
	return "\033[32m" + std::string(text) + "\033[0m";
}


std::string red_text(std::string_view text)
{
	return "\033[31m" + std::string(text) + "\033[0m";
}


std::string yellow_text(std::string_view text)
{
	return "\033[33m" + std::string(text) + "\033[0m";
}


#define PARSER_LOG_ERR \
	red_text("[Parser Error]") << ": "

#define PARSER_LOG_WARN \
	yellow_text("[Parser Warning]") << ": "

#define PARSER_LOG_INFO \
	green_text("[Parser Info]") << ": "