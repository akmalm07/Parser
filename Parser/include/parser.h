#pragma once

#include "include/section_handler.h"
#include "include/rule_handler.h"
#include "include/tokenizer.h"
#include "include/identifier_handler.h"

namespace parser
{

	struct AddWhiteSpace
	{
	public:
		std::string target;
		bool regexBased = false; 
		bool wholeStrOnlyApply = false; // This flag implies whether the check should be applyed to the whole string or if its okay to apply to string parts (substrings)
		bool addIfAlreadyPresent = false; // If the whitespace is already present, should it be added again?
		BeforeOrAfterFlag flag = BeforeOrAfterFlagBits::None;
		AddWhiteSpaceFlags addInPlace = AddWhiteSpaceFlags::None; // This flag indicates what type of whitespace to add in place
	};


	// --String Decoders and Encoders--
	TockenizedUnsectionedFileStr get_str_as_vec(const EntireUntokenizedFile& str, WhiteSpaceDissolveFlag flags = WhiteSpaceDissolveBitFlags::DissolveAll);
	
	std::string get_vec_as_str(const TockenizedUnsectionedFileStr& vec, const std::vector<AddWhiteSpace>& addWhiteSpace, AddWhiteSpaceFlags defaultAdd = AddWhiteSpaceFlags::None);
	


	bool disallow_white_space(WhiteSpaceDissolveFlag flag, char charater);

	std::optional<char> determine_whitespace_addition(BeforeOrAfterFlagBits flag, const std::string& token, const AddWhiteSpace& ws, bool aleadyAdded);


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