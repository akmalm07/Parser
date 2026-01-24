#include "headers.h"
#include "include/regex_ast_parts.h"


namespace parser {

	size_t digits_in_number(unsigned int number)
	{
		size_t digits = 0;
		while (number != 0)
		{
			number /= 10;
			digits++;
		}
		return digits;
	}


	size_t RepeatRangedTimes::charater_length() const
	{
		return 3 + (_min ? digits_in_number(_min.value()) : 0) + 3 + (_max ? digits_in_number(_max.value()) : 0);
	}

} // namespace parser