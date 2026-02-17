#include "headers.h"

#include "include/regex.h"

int main()
{
	parser::Regex regex;
	auto output = regex.compile("(*|4)\\<\\1>");

	std::print("Regex: {}", output);
	//regex.execute("abcf");

    return 0;
}