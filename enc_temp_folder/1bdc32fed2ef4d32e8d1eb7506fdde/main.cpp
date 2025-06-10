#include "headers.h"
#include "include/section_handler.h"
#include "include/tokenizer.h"

int main()
{
	using namespace parser;


	EntireUntokenizedFile fileStr = "This is a test file for the parser section handler";


	EntireTokenizedFile file = tokenize(fileStr, TokenizationSeperationBitFlags::TockeizeWhitespace);

	auto it = file.begin();

	for (int i = 0; i < file.size(); i++)
	{
		std::cout << *(it + i) << "^";
	}


	std::vector<std::shared_ptr<BaseSectioning>> criteria = {
		new_section_when_between_shared<HasIdentifier::No>("a", "file"),
		new_section_when_found_shared<HasIdentifier::No>("the"),
		new_section_when_found_shared<HasIdentifier::No>("parser")
	};

	SectionHandler handler(file, criteria);

	handler.debug_print_sections();
	
	std::cout << green_text("Test completed successfully.") << std::endl;

	return 0;
}