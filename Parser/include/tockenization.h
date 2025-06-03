#pragma once

#include "include/config.h"
#include "include/flags.h"

namespace parser {


	std::vector<std::string> tokenize(std::string_view text);

	std::string print_tokens(const std::vector<std::string_view>& tokens);

	bool check_str(std::string_view str, StringChecksFlag flags);
}