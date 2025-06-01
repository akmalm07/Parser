#pragma once

#include "include/config.h"
#include <fstream>
#include <string_view>
#include <vector>


namespace parser {

	enum class ParserReadType
	{
		Binary,
		Text
	};

	enum class ParserRule
	{
		//Global
		CannotInlcudeInFile,
		MustIncludeInFile,

		//Local
		CannotInlcude,
		MustInclude,
		MustIncludeBefore,
		MustIncludeAfter,
		MustIncludeBetween,

		//Creating Local spaces
		NewSectionWhenFound,
		NewSectionWhenBetween,
	};


	class Parser
	{
	public:

		Parser() = default;

		Parser(std::string_view fileLoc, ParserReadType type);

		template<ParserRule R>
		struct RuleHandler;

		template<>
		struct RuleHandler<ParserRule::CannotInlcudeInFile> 
		{
			static void rule(std::string_view input) 
			{
				std::cout << "Include logic\n";
			}
		};

		// Specialization for Rule::CannotInclude
		template<>
		struct RuleHandler<ParserRule::MustIncludeInFile> 
		{
			static bool check_rule(std::vector<std::string_view> const& sections, std::string_view target, std::string_view errMsg)
			{
				for (auto const& section : sections)
				{
					if (section.find(target) != std::string_view::npos)
					{
						return true;
					}
				}

				std::cerr << "Error: " << errMsg << std::endl;
				return false;
			}
		};

		bool parse(std::string_view fileLoc, ParserReadType type);

		void operator<<(const std::ifstream& file);

	private:

		std::string _fileLocation;
		ParserReadType _readType;
		std::ifstream _fileStream;

		std::vector<std::string_view> _sections;

		void parse_binary(const std::ifstream& file);
		void parse_text(const std::ifstream& file);
		std::ifstream open_file(std::string_view fileLoc, ParserReadType type);

	};
}

#include "include/parser.inl"