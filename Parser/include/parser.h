#pragma once

#include "include/config.h"
#include "include/flags.h"
#include "include/rule_handler.h"

#include <regex>

namespace parser
{

	struct AddWhiteSpace
	{
	public:
		std::string target;
		bool regexBased = false; 
		bool wholeStrOnlyApply = false; // This flag implies whether the check should be applyed to the whole string or if its okay to apply to string parts (substrings)
		BeforeOrAfterFlag flag = BeforeOrAfterFlagBits::None;
		AddWhiteSpaceFlags addInPlace = AddWhiteSpaceFlags::None;
	};

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

		TokenizedSections _sections;

		std::vector<RuleHandler> _rules;

		EntireUntokenizedFile _entireFile;


		void parse_binary(const std::ifstream& file);
		void parse_text(const std::ifstream& file);
		std::ifstream open_file(std::filesystem::path const& fileLoc, ParserReadType type);

	};

	TockenizedUnsectionedFileStr get_str_as_vec(const EntireUntokenizedFile& str, WhiteSpaceDissolveFlag flags = WhiteSpaceDissolveBitFlags::DissolveAll);
	
	bool disallow_white_space(WhiteSpaceDissolveFlag flag, char charater);

	std::string get_vec_as_str(const TockenizedUnsectionedFileStr& vec, const std::vector<AddWhiteSpace>& addWhiteSpace, AddWhiteSpaceFlags defaultAdd = AddWhiteSpaceFlags::None);

	bool disallow_white_space(AddWhiteSpaceFlags flag);

	char add_whitespace(AddWhiteSpaceFlags flag);

	bool str_includes(std::string_view str, std::string_view target);

	struct StrIncludesRegOutput
	{
		bool has;
		std::vector<size_t> startIndices;
		std::vector<size_t> endIndices;

		StrIncludesRegOutput()
			: has(false)
		{}
		
		StrIncludesRegOutput(std::vector<size_t> startIndices, std::vector<size_t> endIndices)
			: startIndices(startIndices), endIndices(endIndices), has(true)
		{
			if (startIndices.size() == 0 || endIndices.size() == 0)
			{
				std::cerr << "\n\nERROR: " <<
					red_text("The Constructor for StrIncludesRegOutput(std::vector<size_t>, std::vector<size_t>) asserts that a vector size of 0 is invalid for any of the inputs ...") << std::endl;
			}
		}



	};
	StrIncludesRegOutput str_includes_reg(const std::string& str, const std::string& target);

}

#include "include/parser.inl"