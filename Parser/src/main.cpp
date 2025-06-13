#include "headers.h"
#include "include/section_handler.h"
#include "include/tokenizer.h"

int main()
{
	using namespace parser;

	

	EntireUntokenizedFile fileStr = R"(
	IF (BOOL1)
	{
		// Do something
		IF (BOOL2)
		{
			// Do something else

		}

	WHILE(BOOL3)
	{
	}

	}
)";
		
	enum STUFF
	{
		BOOL,
		IF,
	};

	EntireTokenizedFile file = tokenize(fileStr, TokenizationSeperationBitFlags::TokenizeBrackets, WhiteSpaceDissolveBitFlags::DissolveAll, true);


	print_tokens(file);
	

	std::vector<std::unique_ptr<BaseSectioning>> criteria;

	criteria.push_back(new_section_when_between_unique<HasIdentifier::Yes>("(", ")", BOOL));
	criteria.push_back(new_section_when_between_unique<HasIdentifier::Yes>("IF (BOOL1) {", "}", IF));


	SectionHandler handler(file, criteria);

	handler.debug_print_sections();
	
	std::cout << green_text("Test completed successfully.") << std::endl;

	return 0;
}