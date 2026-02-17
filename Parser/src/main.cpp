#include "headers.h"

#include "include/regex.h"

int main()
{
	parser::Regex regex;
	auto output = regex.compile("([A-Za])bc\\1");

	std::println("Regex: {}", output);

	if (regex.execute("ZbcZ"))
		std::println("Matched!");
	else
		std::println("Not Matched!");

    return 0;
}