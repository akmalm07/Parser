#pragma once

#include "include/config.h"
#include "include/flags.h"

namespace parser {


	EntireTockenizedFile tokenize(EntireUntockeizedFile const& file, TokenizationSeperationFlag flags, bool isolate = false);
	
	EntireTockenizedFile tokenize(EntireUntockeizedFile const& file, std::vector<std::string> const& items, bool isolate = false);

	EntireUntockeizedFile dissolve_whitespace(EntireUntockeizedFile const& file, WhiteSpaceDissolveFlag flags);
	
	void print_tokens(EntireUntockeizedFile const& tokens);
		
	void print_tokens(TokenizedSection const& tokens);

	//std::string print_tokens(TockenizedSections const& sections);

	bool check_str(std::string_view str, StringChecksFlag flags);

}