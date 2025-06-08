#include "headers.h"
#include "include/section_handler.h"

int main()
{
	using namespace parser;


	TockenizedUnsectionedFile file = { "This", "is", "a", "test", "file", "for", "the", "parser", "section", "handler" };

	std::vector<std::shared_ptr<BaseSectioning>> criteria = {
		NewSectionWhenFoundSectioningShared<HasIdentifier::No>("a"),
		NewSectionWhenFoundSectioningShared<HasIdentifier::No>("the"),
		NewSectionWhenFoundSectioningShared<HasIdentifier::No>("parser")
	};

	SectionHandler handler(file, criteria);

	handler.debug_print_sections();
	
	std::cout << green_text("Test completed successfully.") << std::endl;

	return 0;
}