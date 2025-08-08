#include "headers.h"
#include "include/parser.h"
#include "include/section_handler.h"
#include "include/rule_handler.h"
#include "include/tokenizer.h"
#include "include/identifier_handler.h"

namespace parser 
{

	TockenizedUnsectionedFileStr get_str_as_vec(const EntireUntokenizedFile& str, WhiteSpaceDissolveFlag flags)
	{
		TockenizedUnsectionedFileStr vec;
		std::string token;
		for (char c : str)
		{
			if (disallow_white_space(flags, c) && !token.empty())
			{
				vec.push_back(token);
				token.clear();
			}			
			else
			{
				token += c;
			}
		}
		if (!token.empty())
		{
			vec.push_back(token);
		}

		return vec;
	}
	bool disallow_white_space(WhiteSpaceDissolveFlag flags, char c)
	{
		return (std::isspace(static_cast<int>(c)) && flags & WhiteSpaceDissolveBitFlags::DissolveSpace) ||
			(c == '\n' && flags & WhiteSpaceDissolveBitFlags::DissolveNewLine) ||
			(c == '\t' && flags & WhiteSpaceDissolveBitFlags::DissolveTab) &&
			!(flags & WhiteSpaceDissolveBitFlags::None);
	}

	std::optional<char> determine_whitespace_addition(BeforeOrAfterFlagBits flag, const std::string& token, const AddWhiteSpace& ws, bool aleadyAdded)
	{

		if (ws.flag & flag)
		{
			if (ws.wholeStrOnlyApply && ws.regexBased && 
				std::regex_match(token, std::regex(ws.target)) && 
				(!aleadyAdded || (aleadyAdded && ws.addIfAlreadyPresent)))
			{
				return add_whitespace(ws.addInPlace);
			}
			else if (ws.wholeStrOnlyApply && 
				token == ws.target && 
				(!aleadyAdded || (aleadyAdded && ws.addIfAlreadyPresent)))
			{
				return add_whitespace(ws.addInPlace);
			}
			else if (!ws.wholeStrOnlyApply &&
				!ws.regexBased &&
				str_includes(token, ws.target) &&
				(!aleadyAdded || (aleadyAdded && ws.addIfAlreadyPresent)))
			{
				return add_whitespace(ws.addInPlace);
			}
		}
	}

	// TODO: Unit test later
	std::string get_vec_as_str(const TockenizedUnsectionedFileStr& vec, const std::vector<AddWhiteSpace>& addWhiteSpace, AddWhiteSpaceFlags defaultAdd)
	{
		std::string result;
		bool aleadyAdded = false;
		for (const auto& token : vec)
		{
			for (const auto& ws : addWhiteSpace)
			{

				if (!disallow_white_space(defaultAdd))
				{
					aleadyAdded = true;
					result += add_whitespace(defaultAdd);
				}
				else
				{
					aleadyAdded = false;
				}


				if (!ws.addIfAlreadyPresent)
				{
					result += token;
					continue;
				}

				auto whitespaceAddtionResult = determine_whitespace_addition(BeforeOrAfterFlagBits::Before, token, ws, aleadyAdded);

				if (whitespaceAddtionResult.has_value())
				{
					result += whitespaceAddtionResult.value();
				}

				if (!ws.wholeStrOnlyApply && ws.regexBased) // TODO: Unit test later
				{
					auto indices = str_includes_reg(token, ws.target); 
					
					std::string finalStr;
					size_t lastIndex = 0;
					for (size_t i = 0; i < indices.startIndices.size(); i++)
					{
						finalStr += token.substr(lastIndex, indices.startIndices[i]);
						
						if (ws.flag & BeforeOrAfterFlagBits::Before)
							finalStr += add_whitespace(ws.addInPlace);

						finalStr += token.substr(indices.startIndices[i], indices.endIndices[i] - indices.startIndices[i]);

						if (ws.flag & BeforeOrAfterFlagBits::After)
							finalStr += add_whitespace(ws.addInPlace);

						lastIndex = indices.endIndices[i];
					}
					if (lastIndex < token.size())
						finalStr += token.substr(lastIndex);
				}
				else
				{
					result += token;
				}

				whitespaceAddtionResult = determine_whitespace_addition(BeforeOrAfterFlagBits::After, token, ws, aleadyAdded);

				if (whitespaceAddtionResult.has_value())
				{
					result += whitespaceAddtionResult.value();
				}
			}
		}

		return result;
	} 

	bool disallow_white_space(AddWhiteSpaceFlags flag)
	{ 
		return (flag == AddWhiteSpaceFlags::None);
	}

	char add_whitespace(AddWhiteSpaceFlags flag)
	{
		switch (flag)
		{
		case AddWhiteSpaceFlags::AddNewLine:
			return '\n';
		case AddWhiteSpaceFlags::AddSpace:
			return ' ';
		case AddWhiteSpaceFlags::AddTab:
			return '\t';
		default:
			return ' ';
		}
	}

	//DO UNIT TESTS
	bool str_includes(std::string_view str, std::string_view target)
	{
		size_t offset = 0;
		size_t size = 0;

		if (target.size() > str.size())
			return false;

		for (char c : str)
		{
			if (target.size() != size) 
			{
				size++;
				continue;
			}

			offset++;

			if (str.size() < offset + size)
			{
				return false;
			}

			if (str.substr(offset, size) == target)
			{
				return true;
			}
		}

		return false;
	}

	StrIncludesRegOutput str_includes_reg(const std::string& str, const std::string& target)
	{
		std::regex reg(target); 
			
		auto begin = std::sregex_iterator(str.begin(), str.end(), reg);
		auto end = std::sregex_iterator();

		StrIncludesRegOutput output;

		for (std::sregex_iterator i = begin; i != end; ++i)
		{
			output.startIndices.push_back(i->position()); 
			output.endIndices.push_back(i->position() + i->length()); 
		}

		return output; 
	}



} // namespace parser