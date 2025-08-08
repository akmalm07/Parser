#include "headers.h"
#include "include/tokenizer.h"


namespace parser
{
	EntireTokenizedFile tokenize(EntireUntokenizedFile& file, TokenizationSeperationFlag flags, WhiteSpaceDissolveFlag deleteWhiteSpace)
	{
		return __tokenize__(file, flags, deleteWhiteSpace, true);
	}

	EntireTokenizedFile __tokenize__(EntireUntokenizedFile& text, TokenizationSeperationFlag flags, WhiteSpaceDissolveFlag dissolveWhiteSpace, bool isolate)
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

			if (includes_token_seperator_char(c, flags))
			{
				shouldSplit = true;
			}

			if (shouldSplit)
			{
				if (inToken)
				{
					size_t len = i - tokenStart;
					if (len > 0)
					{
						std::string_view currentToken(text.data() + tokenStart, len);

						bool allDissolvable = true;
						for (char tokenCurrent : currentToken)
						{
							if (!isCharDissolvable(tokenCurrent)) 
							{
								allDissolvable = false;
								break;
							}
						}
						if (!allDissolvable)
						{
							tokens.emplace_back(currentToken);
						}
					}
					inToken = false;
				}

				if (isolate && !isCharDissolvable(c))
				{		
					tokens.emplace_back(text.data() + i, 1);
				}
				else if (!isCharDissolvable(c)) 
				{
					tokenStart = i;
					inToken = true;
				}
			}
			else 
			{
				if (isCharDissolvable(c)) 
				{
					if (inToken) 
					{
						size_t len = i - tokenStart;
						if (len > 0)
						{
							std::string_view currentToken(text.data() + tokenStart, len);
							bool allDissolvable = true;
							for (char tokenCurrent : currentToken) 
							{
								if (!isCharDissolvable(tokenCurrent))
								{
									allDissolvable = false;
									break;
								}
							}
							if (!allDissolvable)
							{
								tokens.emplace_back(currentToken);
							}
						}
						inToken = false;
					}
				}
				else 
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
				for (char tokenCurrent : lastToken) 
				{
					if (!isCharDissolvable(tokenCurrent))
					{
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

	bool includes_token_seperator_char(char c, TokenizationSeperationFlag flags)
	{
		if (
			(flags.has(TokenizationSeperationBitFlags::TockeizeSpace) && std::isspace(static_cast<unsigned char>(c))) ||
			(flags.has(TokenizationSeperationBitFlags::TockeizeNewLine) && c == '\n') ||
			(flags.has(TokenizationSeperationBitFlags::TockeizeTab) && c == '\t') ||
			(flags.has(TokenizationSeperationBitFlags::TockeizeAtoZ) && std::isalpha(static_cast<unsigned char>(c))) ||
			(flags.has(TokenizationSeperationBitFlags::TockeizeDigit) && std::isdigit(static_cast<unsigned char>(c))) ||
			(flags.has(TokenizationSeperationBitFlags::TockeizeExplemation) && c == '!') ||
			(flags.has(TokenizationSeperationBitFlags::TockeizeQuestion) && c == '?') ||
			(flags.has(TokenizationSeperationBitFlags::TockeizeColon) && c == ':') ||
			(flags.has(TokenizationSeperationBitFlags::TockeizeSemicolon) && c == ';') ||
			(flags.has(TokenizationSeperationBitFlags::TockeizeDot) && c == '.') ||
			(flags.has(TokenizationSeperationBitFlags::TockeizeComma) && c == ',') ||
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
			(flags.has(TokenizationSeperationBitFlags::TokenizeOpenBracket) && c == '(') ||
			(flags.has(TokenizationSeperationBitFlags::TokenizeClosedBracket) && c == ')') ||
			(flags.has(TokenizationSeperationBitFlags::TokenizeOpenCurlyBracket) && c == '{') ||
			(flags.has(TokenizationSeperationBitFlags::TokenizeClosedCurlyBracket) && c == '}') ||
			(flags.has(TokenizationSeperationBitFlags::TokenizeOpenSquareBracket) && c == '[') ||
			(flags.has(TokenizationSeperationBitFlags::TokenizeClosedSquareBracket) && c == ']')
			)
		{
			return true;
		}
		return false;
	}

	bool includes_token_seperator_str(std::string_view str, std::string_view target)
	{
		if (str.size() < target.size() && str.find(target) != std::string::npos)
		{
			return true;
		}

		return false;
	}

	//** IN PROGRESS **//



	std::vector<TargetMatch> find_all_matching_indices_str(const std::string& mainStr, const std::string& target)
	{
		std::vector<TargetMatch> matches;
		size_t currentPos = 0; 

		while (size_t foundPos = mainStr.find(target, currentPos) != std::string::npos)
		{
			size_t endPos = foundPos + target.length();

			matches.push_back({ foundPos, endPos });

			currentPos = foundPos + target.length();

			if (target.empty())
			{
				currentPos++;
			}
		}
		return matches;
	}


	EntireTokenizedFile tokenize(EntireUntokenizedFile& file, std::vector<std::string> const& targets, WhiteSpaceDissolveFlag deleteWhiteSpace)
	{
		std::vector<std::pair<std::string_view, size_t>> tokens;
		tokens.reserve(file.size() / 2);

		size_t currentFilePos = 0; 
		size_t tokenStart = 0;

		auto isCharDissolvable = [&](char c) -> bool
			{
				if (c == ' ' && deleteWhiteSpace.has(WhiteSpaceDissolveBitFlags::DissolveSpace))
					return true;
				if (c == '\n' && deleteWhiteSpace.has(WhiteSpaceDissolveBitFlags::DissolveNewLine))
					return true;
				if (c == '\t' && deleteWhiteSpace.has(WhiteSpaceDissolveBitFlags::DissolveTab))
					return true;
				return false;
			};

		auto addTokenIfValid = [&](size_t startIndex, size_t endIndex)
			{
				if (startIndex < endIndex)
				{
					std::string_view currentSegment(file.data() + startIndex, endIndex - startIndex);
					bool allDissolvable = true;
					for (char c : currentSegment)
					{
						if (!isCharDissolvable(c))
						{
							allDissolvable = false;
							break;
						}
					}
					if (!allDissolvable) 
					{
						tokens.emplace_back(currentSegment, startIndex);
					}
				}
			};

		while (currentFilePos < file.size())
		{
			bool targetMatched = false;
			size_t matchedTargetLength = 0;

			if (isCharDissolvable(file[currentFilePos]))
			{
				addTokenIfValid(tokenStart, currentFilePos); 
				tokenStart = currentFilePos + 1;             
				currentFilePos++;
				continue; 
			}

			for (const auto& target : targets)
			{
				if (currentFilePos + target.size() <= file.size() && file.compare(currentFilePos, target.size(), target) == 0)
				{
					addTokenIfValid(tokenStart, currentFilePos);

					bool targetIsEntirelyDissolvable = true;
					for (char c : target)
					{
						if (!isCharDissolvable(c))
						{
							targetIsEntirelyDissolvable = false;
							break;
						}
					}

					if (!targetIsEntirelyDissolvable) 
					{
						tokens.emplace_back(std::string_view(file.data() + currentFilePos, target.size()), currentFilePos);
					}

					tokenStart = currentFilePos + target.size(); 
					currentFilePos += target.size();             
					targetMatched = true;
					break; 
				}
			}

			if (!targetMatched)
			{
				currentFilePos++; 
			}
		}

		addTokenIfValid(tokenStart, file.size());


		std::sort(tokens.begin(), tokens.end(),
			[](const std::pair<std::string_view, size_t>& a,
				const std::pair<std::string_view, size_t>& b)
			{
				return a.second < b.second;
			});

		std::vector <std::string_view> resultTokens;
		for (auto& token : tokens)
		{
			if (!token.first.empty())
			{
				resultTokens.emplace_back(token.first);
			}	
		}

		return resultTokens;
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

	void print_tokens(TokenizedSectionizedCompact const& sections)
	{
		for (const auto& coord : sections.coords)
		{
			std::cout << "Section from " << coord.start << " to " << coord.end << " with identifier: " << coord.identifier << ": ";
			for (size_t i = coord.start; i < coord.end; i++)
			{
				std::cout << sections.tokens[i] << " ";
			}
			std::cout << "\n"; // New line for each section
			std::cout << std::endl;
		}
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