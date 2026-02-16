#pragma once

#include "config.h"

#include "regex_ast_parts.h"


namespace parser
{
	
	class Regex 
	{
	public:
		Regex() = default;
		
		std::string compile(std::string_view pattern);

		std::string print_regex_compilation() const;

		bool execute(std::string_view test);
		
		~Regex() = default;
	private:
		std::vector<std::unique_ptr<BasePart>> _parts;
	};
}


