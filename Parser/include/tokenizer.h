#pragma once

#include "include/config.h"
#include "include/flags.h"

namespace parser {


	EntireTokenizedFile tokenize(EntireUntokenizedFile& file, TokenizationSeperationFlag flags, WhiteSpaceDissolveFlag deleteWhiteSpace = WhiteSpaceDissolveBitFlags::DissolveAll, bool isolate = false);
	

	EntireUntokenizedFile dissolve_whitespace(EntireUntokenizedFile const& file, WhiteSpaceDissolveFlag flags = WhiteSpaceDissolveBitFlags::DissolveAll);
			
	void print_tokens(EntireUntokenizedFile const& tokens);
		
	void print_tokens(TokenizedSection const& tokens);

	//std::string print_tokens(TockenizedSections const& sections);

	bool check_str(std::string_view str, StringChecksFlag flags);

}