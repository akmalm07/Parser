#pragma once

#include "include/config.h"


namespace parser
{

	class Parser
	{
	public:

		Parser() = default;

		Parser(std::string_view fileLoc, ParserReadType type);


		bool parse(std::string_view fileLoc, ParserReadType type);

		void operator<<(const std::ifstream& file);

	private:

		std::filesystem::path _fileLocation;
		ParserReadType _readType;
		std::ifstream _fileStream;

		TockenizedSections _sections;

		std::vector<Rule> _rules;

		TockenizedFile _entireFile;

		void parse_binary(const std::ifstream& file);
		void parse_text(const std::ifstream& file);
		std::ifstream open_file(std::string_view fileLoc, ParserReadType type);

	};
}

#include "include/parser.inl"