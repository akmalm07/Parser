#pragma once

#include <iostream>
#include <string>


#include <fstream>
#include <string_view>
#include <vector>
#include <array>
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
		NewSectionWhenFound, // 1 param
		NewSectionWhenBetween, // 2 param
		NewSectionWhenAfter, // 2 param
		NewSectionWhenBefore, // 2 param

	};


	/*
	struct Rule
	{
		ParserRule type;
		std::string errMsg;
		virtual Rule* get() = 0;
		virtual ~Rule() = default;
	};

	*/


	struct BaseRule
	{
		ParserRule type;
		std::string errMsg;

		virtual size_t get_target_count() const = 0;
	};

	template<size_t N>
		requires (N > 0)
	struct Rule : public BaseRule
	{
		std::array<std::string, N> targets; // This is a fixed size array of strings that contains the targets of the rule. The size of the array is determined by the template parameter N.
		
		size_t get_target_count() const override { return N; }

		Rule(ParserRule r, std::array<std::string, N> const& t, std::string_view e)
		{
			type = r;
			targets = t;
			errMsg = e;
		}

		~Rule() = default;

	};

	template<>
	struct Rule<1> : BaseRule
	{
		std::string target;

		size_t get_target_count() const override { return 1; }

		Rule(ParserRule r, std::string_view t, std::string_view e)
		{
			type = r;
			target = t;
			errMsg = e;
		}
		
		~Rule() = default;
	};

	using RuleOneTarget = Rule<1>;
	using RuleTwoTarget = Rule<2>;

	template<size_t N>
	using RuleMultiTarget = Rule<N>;



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