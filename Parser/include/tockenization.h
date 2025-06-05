#pragma once

#include "include/config.h"
#include "include/flags.h"

namespace parser {


	std::vector<std::string> tokenize(std::string_view text, TokenizationSeperationFlag flags);

	std::string dissolve_whitespace(std::string_view text, WhiteSpaceDissolveFlag flags);
	
	std::vector<std::string> dissolve_whitespace(std::vector<std::string> const& text, WhiteSpaceDissolveFlag flags);

	std::string print_tokens(TockenizedUnsectionedFile const& tokens);

	//std::string print_tokens(TockenizedSections const& sections);

	bool check_str(std::string_view str, StringChecksFlag flags);

}