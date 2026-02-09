#include "headers.h"

#include "include/regex.h"

int main()
{
	parser::Regex regex;
							//   0          11 'c'
	auto output = regex.compile("([Aa]|[bB])(123)*a|2");

	std::print("Regex: {}", output);
	//regex.execute("abcf");

    return 0;
}