#include "include/parser.h"


namespace parser {
	// Constructor
	Parser::Parser(const std::string& fileLoc, ParserReadType type)
		: _fileLocation(fileLoc), _readType(type) 
	{
		_fileStream = open_file(fileLoc, type);
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
	std::ifstream Parser::open_file(std::string_view fileLoc, ParserReadType type)
	{
		std::ios_base::openmode mode = (type == ParserReadType::Binary) ? std::ios::binary : std::ios::in;
		_readType = type;

		std::ifstream file(fileLoc.data(), mode);

		if (!file.is_open()) 
		{
			std::cerr << PARSER_LOG_ERR << "Failed to open file: " << fileLoc.data() << "\n";
		}

		return file;
	}
	void Parser::parse_binary(const std::ifstream& file) 
	{
		
	}
	void Parser::parse_text(const std::ifstream& file)
	{
		
	}
} // namespace parser