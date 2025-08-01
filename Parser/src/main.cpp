#include "headers.h"
#include "include/section_handler.h"
#include "include/rule_handler.h"
#include "include/tokenizer.h"

int main()
{
	using namespace parser;

	

	EntireUntokenizedFile fileStr = R"(
		IF (BOOL1) {
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
		
	enum Identifiers : size_t
	{
		BOOL = 1ULL << 0,
		IF = 1ULL << 1,
	};



	EntireTokenizedFile file = tokenize(fileStr, TokenizationSeperationBitFlags::TokenizeBrackets, WhiteSpaceDissolveBitFlags::DissolveAll, true);


	print_tokens(file);
	

	std::vector<std::unique_ptr<BaseSectioning>> criteria;

	criteria.push_back(new_section_when_between_unique("(", ")", BOOL));
	criteria.push_back(new_section_when_between_unique("{", "}", IF));


	SectionHandler handler(file, criteria);

	handler.debug_print_sections();

	std::vector<std::unique_ptr<Rule>> rules;

	rules.push_back(new_rule_must_include_in_file_unique("BOOL1"));
	rules.push_back(new_rule_must_include_unique("BOOL1", BOOL));
	rules.push_back(new_rule_must_include_in_file_unique("WHILE|while"));

	RuleHandler ruleHandler(rules);

	if (!ruleHandler.check_rules(handler.get_compressed_sections()))
	{
		std::cerr << red_text("Rule check failed.") << std::endl;
		return 1;
	}



	std::cout << green_text("Test completed successfully.") << std::endl;

	return 0;
}