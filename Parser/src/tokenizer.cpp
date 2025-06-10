#include "headers.h"
#include "include/tokenizer.h"


namespace parser
{
	EntireTockenizedFile tokenize(EntireUntockeizedFile const& text, TokenizationSeperationFlag flags, bool isolate)
	{
		std::vector<std::string_view> tokens;
		std::string current;



		for (auto const& c : text)
		{	
			current += c;
			if (c == '\0') 
				continue;

			bool addSpace = false;
			bool shouldSplit = false;

			if (flags.has(TokenizationSeperationBitFlags::TockeizeWhitespace) && (std::isspace(static_cast<unsigned char>(c)) || c == '\n' || c == '\t'))
				shouldSplit = true;
			
			if (flags.has(TokenizationSeperationBitFlags::TockeizeSpace) && std::isspace(static_cast<unsigned char>(c)))
				shouldSplit = true;

			if (flags.has(TokenizationSeperationBitFlags::TockeizeNewLine) && c == '\n')
				shouldSplit = true;

			if (flags.has(TokenizationSeperationBitFlags::TockeizeTab) && c == '\t')
				shouldSplit = true;

			if (flags.has(TokenizationSeperationBitFlags::TockeizeAtoZ) && std::isalpha(static_cast<unsigned char>(c)))
				shouldSplit = true;

			if (flags.has(TokenizationSeperationBitFlags::TockeizeDigit) && std::isdigit(static_cast<unsigned char>(c)))
				shouldSplit = true;

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
				shouldSplit = true;
			}

			if (shouldSplit)
			{
				if (isolate)
				{
					tokens.emplace_back(current.substr(0, current.size() - 1));
					tokens.emplace_back(&current.back(), 1);
				}
				else
				{
					tokens.emplace_back(current);
				}
				current.clear();
			}

		}

		if (!current.empty())
			tokens.emplace_back(current);
		current.clear();

		return tokens;
	}

	EntireTockenizedFile tokenize(EntireUntockeizedFile const& file, std::vector<std::string> const& items, bool isolate)
	{
		EntireTockenizedFile tokens;
		tokens.reserve(file.size() / 2);
		std::string current;

		for (char c : file)
		{
			current += c;

			bool matched = false;
			for (const auto& item : items)
			{
				if (isolate && current.size() >= item.size() &&
					current.substr(current.size() - item.size()) == item)
				{
					tokens.emplace_back(current.substr(0, current.size() - item.size()));
					tokens.emplace_back(item);
					current.clear();
					matched = true;
					break;
				}
				else if (!isolate && current == item)
				{
					tokens.emplace_back(current);
					current.clear();
					matched = true;
					break;
				}
			}

			if (matched)
				continue;
		}

		if (!current.empty())
			tokens.emplace_back(current);

		return tokens;
	}


	std::string dissolve_whitespace(EntireUntockeizedFile const& text, WhiteSpaceDissolveFlag flags)
	{
		std::string result;
		for (char const c : text)
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



	void print_tokens(EntireUntockeizedFile const& tokens)
	{
		for (const auto& token : tokens)
		{
			std::cout << token << " ";
		}
		std::cout << std::endl;
	}


	void print_tokens(TokenizedSection const& tokens)
	{
		for (const auto& token : tokens)
		{
			std::cout << token << " ";
		}
		std::cout << std::endl;
	}


	/*
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

	*/


}