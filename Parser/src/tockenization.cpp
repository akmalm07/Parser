#include "include/tockenization.h"


namespace parser
{
	std::vector<std::string> tokenize(std::string_view text, TokenizationSeperationFlag flags)
	{
		std::vector<std::string> tokens;
		std::string current;

		for (char c : text)
		{
			bool should_split = false;

			if (flags.has(TokenizationSeperationBitFlags::TockeizeWhitespace) && std::isspace(static_cast<unsigned char>(c)) && c != '\n' && c != '\t')
				should_split = true;

			if (flags.has(TokenizationSeperationBitFlags::TockeizeNewLine) && c == '\n')
				should_split = true;

			if (flags.has(TokenizationSeperationBitFlags::TockeizeTab) && c == '\t')
				should_split = true;

			if (flags.has(TokenizationSeperationBitFlags::TockeizeAtoZ) && std::isalpha(static_cast<unsigned char>(c)))
				should_split = true;

			if (flags.has(TokenizationSeperationBitFlags::TockeizeDigit) && std::isdigit(static_cast<unsigned char>(c)))
				should_split = true;

			// Punctuation checks
			if ((flags.has(TokenizationSeperationBitFlags::TockeizeExplemation) && c == '!') ||
				(flags.has(TokenizationSeperationBitFlags::TockeizeQuestion) && c == '?') ||
				(flags.has(TokenizationSeperationBitFlags::TockeizeColon) && c == ':') ||
				(flags.has(TokenizationSeperationBitFlags::TockeizeSemicolon) && c == ';') ||
				(flags.has(TokenizationSeperationBitFlags::TockeizeDot) && c == '.') ||
				(flags.has(TokenizationSeperationBitFlags::TockeizeComma) && c == ',') ||

				// Special symbols
				(flags.has(TokenizationSeperationBitFlags::TokenizeAt) && c == '@') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeHash) && c == '#') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeDollar) && c == '$') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizePercent) && c == '%') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeCaret) && c == '^') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeAmpersand) && c == '&') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeStar) && c == '*') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeDash) && c == '-') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizePlus) && c == '+') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeEqual) && c == '=') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeSlash) && c == '/') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeBackslash) && c == '\\') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeTilde) && c == '~') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizePipe) && c == '|') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeGrave) && c == '`') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeUnderscore) && c == '_') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeOpenAngleBracket) && c == '<') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeClosedAngleBracket) && c == '>') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeOpenBracket) && c == '(' ) ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeClosedBracket) && c == ')') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeOpenCurlyBracket) && c == '{') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeClosedCurlyBracket) && c == '}') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeOpenSquareBracket) && c == '[') ||
				(flags.has(TokenizationSeperationBitFlags::TokenizeClosedSquareBracket) && c == ']'))
			{
				should_split = true;
			}

			if (should_split)
			{
				tokens.push_back(std::move(current));
				current.clear();
			}
			else
			{
				current += c;
			}
		}

		tokens.push_back(std::move(current));
		current.clear();

		return tokens;
	}

	std::string dissolve_whitespace(std::string_view text, WhiteSpaceDissolveFlag flags)
	{
		std::string result;
		for (char c : text)
		{
			if (flags.has(WhiteSpaceDissolveBitFlags::DissolveSpace) && std::isspace(static_cast<unsigned char>(c)) && c != '\n' && c != '\t')
			{
				continue; 
			}
			if (flags.has(WhiteSpaceDissolveBitFlags::DissolveNewLine) && c == '\n')
			{
				continue; 
			}
			if (flags.has(WhiteSpaceDissolveBitFlags::DissolveTab) && c == '\t')
			{
				continue; 
			}
			result += c; 
		}
		return result;
	}

	std::vector<std::string> dissolve_whitespace(std::vector<std::string> const& text, WhiteSpaceDissolveFlag flags)
	{

		std::vector<std::string> result;
		for (const auto& str : text)
		{
			result.push_back(dissolve_whitespace(str, flags));
		}
		return result;
	}


	bool check_str(std::string_view str, StringChecksFlag flags)
	{
		if (str.empty())
		{
			return false;
		}

		for (char c : str)
		{
			if (!flags.has(StringChecksBitFlags::NoDigitCheck) && isdigit(static_cast<unsigned char>(c)))
			{
				return false;
			}

			if (!flags.has(StringChecksBitFlags::NoSpecialCharCheck) && !isalnum(static_cast<unsigned char>(c)))
			{
				return false;
			}

			if (!flags.has(StringChecksBitFlags::NoAtoZCheck) && isalpha(static_cast<unsigned char>(c)))
			{
				return false;
			}

			if (!flags.has(StringChecksBitFlags::NoSpaceCheck) && isspace(static_cast<unsigned char>(c)))
			{
				return false;
			}

			if (!flags.has(StringChecksBitFlags::NoNewLineCheck) && c == '\n')
			{
				return false;
			}

			if (!flags.has(StringChecksBitFlags::NoTabCheck) && c == '\t')
			{
				return false;
			}
		}

		return true;
	}



	std::string print_tokens(TockenizedSection const& tokens)
	{
		std::ostringstream oss;
		for (const auto& token : tokens)
		{
			oss << token << " ";
		}
		return oss.str();
	}


	std::string print_tokens(TockenizedSections const& sections)
	{
		std::ostringstream oss;
		for (const auto& section : sections)
		{
			for (const auto& token : section)
			{
				oss << token << " ";
			}
			oss << "\n"; // New line for each section
		}
		return oss.str();
	}




}