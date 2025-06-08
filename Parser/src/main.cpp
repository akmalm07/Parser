#include "headers.h"
#include "include/section_handler.h"

int main()
{
	using namespace parser;


	TockenizedUnsectionedFile file = { "This", "is", "a", "test", "file", "for", "the", "parser", "section", "handler" };

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