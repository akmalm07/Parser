#include "include/parser.h"


namespace parser {
	
	template <ParserRule T>
	void Parser::rule(ParserRule rule) {}


	template <ParserRule T>
	void Parser::rule<ParserRule::CannotInlcudeInFile>(ParserRule rule, std::string_view str)
	{
	}


} // namespace parser