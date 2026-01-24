#pragma once

#include "config.h"

#include "regex_ast_parts.h"

namespace parser
{
	
	class Regex 
	{
	public:
		Regex() = default;
		
		unsigned int compile(std::string_view pattern);
		
		~Regex() = default;
	private:
		std::vector<std::unique_ptr<BasePart>> _parts;
	};
}


