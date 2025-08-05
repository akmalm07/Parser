#pragma once

#include "include/config.h"
#include "include/flags.h"

namespace parser 
{

	struct TargetMatch
	{
		size_t startIndex;
		size_t endIndex;
	};


	EntireTokenizedFile tokenize(EntireUntokenizedFile& file, TokenizationSeperationFlag flags, WhiteSpaceDissolveFlag deleteWhiteSpace = WhiteSpaceDissolveBitFlags::DissolveAll);
	EntireTokenizedFile tokenize(EntireUntokenizedFile& file, std::vector<std::string> const& targets, WhiteSpaceDissolveFlag deleteWhiteSpace = WhiteSpaceDissolveBitFlags::DissolveAll);


	// The Isolated flag is depriciated, but supported witht he __ suffix and prefix. by default, the isolated flag is true
	EntireTokenizedFile __tokenize__(EntireUntokenizedFile& file, TokenizationSeperationFlag flags, WhiteSpaceDissolveFlag deleteWhiteSpace = WhiteSpaceDissolveBitFlags::DissolveAll, bool isolate = false);

	bool includes_token_seperator_char(char c, TokenizationSeperationFlag flags);
	
	std::vector<TargetMatch> find_all_matching_indices_str(const std::string& mainStr, const std::string& target);

	bool includes_token_seperator_str(std::string_view str, std::string_view target);

	EntireUntokenizedFile dissolve_whitespace(EntireUntokenizedFile const& file, WhiteSpaceDissolveFlag flags = WhiteSpaceDissolveBitFlags::DissolveAll);
			
	void print_tokens(EntireUntokenizedFile const& tokens);
		
	void print_tokens(TokenizedSection const& tokens);

	void print_tokens(TokenizedSectionizedCompact const& sections);

	//std::string print_tokens(TockenizedSections const& sections);

	bool check_str(std::string_view str, StringChecksFlag flags);

}