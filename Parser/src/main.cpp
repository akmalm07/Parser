#include "headers.h"

#include "include/regex.h"
class Class {};
int main()
{
	parser::Regex regex;
	auto output = regex.compile("a(b(c)|(d)e)f");

	std::print("Regex: {}", output);
	//regex.execute("abcf");

    return 0;
}