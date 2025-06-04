#pragma once

#include "include/config.h"
#include "include/flags.h"
#include "include/rule_handler.h"

namespace parser
{

	class Parser
	{
	public:

		Parser() = default;

		Parser(std::filesystem::path const& fileLoc, ParserReadType type);

		void set_rules(std::vector<RuleHandler> const& rules);

		bool parse(std::string_view fileLoc, ParserReadType type);

		void operator<<(const std::ifstream& file);

	private:

		std::filesystem::path _fileLocation;
		ParserReadType _readType;
		std::ifstream _fileStream;

		TockenizedSections _sections;

		std::vector<RuleHandler> _rules;

		TockenizedFile _entireFile;

		void parse_binary(const std::ifstream& file);
		void parse_text(const std::ifstream& file);
		std::ifstream open_file(std::filesystem::path const& fileLoc, ParserReadType type);

	};
}

#include "include/parser.inl"