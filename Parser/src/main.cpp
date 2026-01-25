#include "headers.h"

#include "include/regex.h"
int main()
{
	parser::Regex regex;
	regex.compile("a(b(c)|(d)e)f");

	regex.execute("abcf");

    return 0;
}