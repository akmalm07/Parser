#pragma once

#include "config.h"

#include "regex_parts.h"

namespace parser
{
	enum class ErrCode
	{
		None = 0,
		UnmatchedClosingParenthesis,
		InvalidEscapeSequence,
		UnexpectedEndOfPattern,
		InvalidQuantifierPosition,
		UnmatchedOpeningParenthesis,
		UnknownError
	};

	std::expected<std::vector<std::unique_ptr<BasePart>>, ErrCode> regex_parser(const std::string& pattern);

}


