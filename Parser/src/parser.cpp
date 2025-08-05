#include "headers.h"
#include "include/parser.h"


namespace parser {
	// Constructor
	Parser::Parser(std::filesystem::path const& fileLoc, ParserReadType type)
		: _fileLocation(fileLoc), _readType(type) 
	{
		_fileStream = open_file(fileLoc, type);
	}

	void Parser::set_rules(std::vector<RuleHandler> const& rules)
	{
		if (_rules.empty()) 
		{
			std::cerr << PARSER_LOG_ERR << "No rules set for parser." << std::endl;
		}
		else 
		{
			std::cout << PARSER_LOG_INFO << "Rules set for parser. Number of rules set: " << _rules.size() << std::endl;
		}
	}

	bool Parser::parse(std::string_view fileLoc, ParserReadType type)
	{
		_fileStream = open_file(fileLoc, type);
		if (!_fileStream.is_open()) {
			return false;
		}

		if (type == ParserReadType::Binary) 
		{
			parse_binary(_fileStream);
		}
		else 
		{
			parse_text(_fileStream);
		}

		return true;
	}

	void Parser::operator<<(const std::ifstream& file) 
	{
		
			parse_text(file);

	}
	std::ifstream Parser::open_file(std::filesystem::path const& fileLoc, ParserReadType type)
	{
		std::ios_base::openmode mode = (type == ParserReadType::Binary) ? std::ios::binary : std::ios::in;
		_readType = type;

		std::ifstream file(fileLoc, mode);

		if (!file.is_open()) 
		{
			std::cerr << PARSER_LOG_ERR << "Failed to open file: " << fileLoc << "\n";
		}

		return file;
	}
	void Parser::parse_binary(const std::ifstream& file) 
	{

	}
	void Parser::parse_text(const std::ifstream& file)
	{
		
	}
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

	std::string get_vec_as_str(const TockenizedUnsectionedFileStr& vec, const std::vector<AddWhiteSpace>& addWhiteSpace, AddWhiteSpaceFlags defaultAdd)
	{
		std::string result;
		bool aleadyAdd = false;
		for (const auto& token : vec)
		{
			for (const auto& ws : addWhiteSpace)
			{
				aleadyAdd = false;

				if (defaultAdd == AddWhiteSpaceFlags::AddSpace) 
				{
					result += " ";
				}

				if (disallow_white_space(ws.addInPlace))
					goto SKIP_EXTRA_WHITESPACE_ADD_FROM_GET_VEC_AS_STR;

				if (ws.wholeStrOnlyApply && 
					ws.flag & BeforeOrAfterFlagBits::Before) 
				{
					if (ws.regexBased)
					{
						if (std::regex_match(token, std::regex(ws.target)))
						{
							result += add_whitespace(ws.addInPlace); // Add space before
						}
					}
					else
					{
						if (token == ws.target) 
						{
							result += add_whitespace(ws.addInPlace);
							aleadyAdd = true;
						}
					}
				}
				else if (ws.flag & BeforeOrAfterFlagBits::Before)
				{
					if (ws.regexBased)
					{
						auto indices = str_includes_reg(token, ws.target); 
						for (size_t i = 0; i < indices.startIndices.size(); ++i)
						{
							if (indices.startIndices[i] == 0)
							{
								result += add_whitespace(ws.addInPlace);
								aleadyAdd = true;
								break;
							}
						}
					}
					else
					{
						
					}
				}
				


				result += token;

				SKIP_EXTRA_WHITESPACE_ADD_FROM_GET_VEC_AS_STR:

				if (defaultAdd == AddWhiteSpaceFlags::AddSpace)
				{
					result += " ";
				}


				if (disallow_white_space(defaultAdd))
				{
					result += " "; // Default space after each token
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