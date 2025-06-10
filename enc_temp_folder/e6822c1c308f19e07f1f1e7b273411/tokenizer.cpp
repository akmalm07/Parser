#include "headers.h"
#include "include/tokenizer.h"


namespace parser
{
	EntireTokenizedFile tokenize(EntireUntokenizedFile& text, TokenizationSeperationFlag flags, WhiteSpaceDissolveFlag dissolveWhiteSpace, bool isolate)
	{
		std::vector<std::string_view> tokens;
		size_t tokenStart = 0;
		bool inToken = false;


		size_t estimatedSize = text.size() / 2;
		tokens.reserve(estimatedSize);


		auto isCharDissolvable = [&dissolveWhiteSpace](char c) -> bool
			{
				if (c == ' ' && dissolveWhiteSpace.has(WhiteSpaceDissolveBitFlags::DissolveSpace))
					return true;
				if (c == '\n' && dissolveWhiteSpace.has(WhiteSpaceDissolveBitFlags::DissolveNewLine))
					return true;
				if (c == '\t' && dissolveWhiteSpace.has(WhiteSpaceDissolveBitFlags::DissolveTab))
					return true;
				return false;
			};


		for (size_t i = 0; i < text.size(); i++)
		{
			char c = text[i];

			if (c == '\0') 
				continue;

			bool shouldSplit = false;

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
				// If we were in a token, end it and add it if it's not entirely dissolvable whitespace
				if (inToken)
				{
					size_t len = i - tokenStart;
					if (len > 0)
					{
						std::string_view currentToken(text.data() + tokenStart, len);
						// Check if *any* character in the currentToken is NOT dissolvable whitespace.
						// If all characters are dissolvable, then dissolve the whole token.
						bool allDissolvable = true;
						for (char tok_c : currentToken) {
							if (!isCharDissolvable(tok_c)) {
								allDissolvable = false;
								break;
							}
						}
						if (!allDissolvable) {
							tokens.emplace_back(currentToken);
						}
					}
					inToken = false;
				}

				// Handle the current character `c` itself, especially if `isolate` is true
				if (isolate)
				{
					// Only add the isolated character if it's not a dissolvable whitespace character
					if (!isCharDissolvable(c))
					{
						tokens.emplace_back(text.data() + i, 1);
					}
				}
				else
				{
					// If not isolating, this character becomes the start of a potential new token,
					// but only if it's not a dissolvable whitespace character.
					if (!isCharDissolvable(c)) {
						tokenStart = i;
						inToken = true;
					}
					else {
						// If it's a dissolvable whitespace char and not isolated, it just gets skipped.
						// `inToken` remains false, `tokenStart` doesn't advance.
					}
				}
			}
			else // Current character `c` does NOT cause a split
			{
				// If it's a dissolvable whitespace character, we skip it
				if (isCharDissolvable(c)) {
					// If we were in a token, finalize it first, then skip this whitespace
					if (inToken) {
						size_t len = i - tokenStart;
						if (len > 0) {
							std::string_view currentToken(text.data() + tokenStart, len);
							bool allDissolvable = true;
							for (char tok_c : currentToken) {
								if (!isCharDissolvable(tok_c)) {
									allDissolvable = false;
									break;
								}
							}
							if (!allDissolvable) {
								tokens.emplace_back(currentToken);
							}
						}
						inToken = false; // End the current token
					}
					// Then continue to the next character, effectively dissolving `c`
				}
				else // Current character `c` is not a split character AND not dissolvable whitespace
				{
					if (!inToken)
					{
						tokenStart = i;
						inToken = true;
					}
				}
			}
		}

		if (inToken)
		{
			size_t len = text.size() - tokenStart;
			if (len > 0)
			{
				std::string_view lastToken(text.data() + tokenStart, len);
				bool allDissolvable = true;
				for (char tok_c : lastToken) {
					if (!isCharDissolvable(tok_c)) {
						allDissolvable = false;
						break;
					}
				}
				if (!allDissolvable) {
					tokens.emplace_back(lastToken);
				}
			}
		}

		return tokens;
	}

	EntireTokenizedFile tokenize(std::string const& file, std::vector<std::string> const& items, WhiteSpaceDissolveFlag deleteWhiteSpace, bool isolate)
	{
		std::vector<std::string_view> tokens;
		tokens.reserve(file.size() / 2);

		size_t start = 0;  
		size_t i = 0;      
		while (i < file.size())
		{
			bool matched = false;

			for (const auto& item : items)
			{
				if (isolate)
				{
					if (i + 1 >= item.size() && file.compare(i + 1 - item.size(), item.size(), item) == 0)
					{
						if (start < i + 1 - item.size())
							tokens.emplace_back(file.data() + start, i + 1 - item.size() - start);

						tokens.emplace_back(item);

						start = i + 1;
						matched = true;
						break;
					}
				}
				else
				{
					if (i - start + 1 == item.size() && file.compare(start, item.size(), item) == 0)
					{
						tokens.emplace_back(item);
						start = i + 1;
						matched = true;
						break;
					}
				}
			}

			if (matched)
			{
				i = start;
			}
			else
			{
				i++;
			}
		}

		if (start < file.size())
			tokens.emplace_back(file.data() + start, file.size() - start);

		return tokens;
	}


	EntireUntokenizedFile dissolve_whitespace(EntireUntokenizedFile const& text, WhiteSpaceDissolveFlag flags)
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



	void print_tokens(EntireUntokenizedFile const& tokens)
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