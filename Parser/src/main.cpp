#include "headers.h"
#include "include/section_handler.h"
#include "include/rule_handler.h"
#include "include/tokenizer.h"
#include "include/parser.h"
#include "include/identifier_handler.h"

int main()
{
	using namespace parser;

	

	// FIX THE TOKENIZER, as it did
	EntireUntokenizedFile fileStr = R"( 
		int func
		{
			bool var = true;
			while (i < j)
			{
				if ()
				{
					var = false;
				}
			}
		}
	)";
		
	enum Identifiers : size_t
	{
		VARIUBLE = 1ULL << 0,
		FUNCTIONCALL = 1ULL << 1,
		BOOL = 1ULL << 2,
	};

	enum IdentifiersBlocks : size_t
	{
		PEREN = 1ULL << 0,
		CURLY = 1ULL << 1,
		IF = 1ULL << 2,
	};

	
	EntireTokenizedFile file = tokenize(fileStr, TokenizationSeperationBitFlags::TokenizeBrackets, WhiteSpaceDissolveBitFlags::DissolveAll);
	

	std::vector<std::unique_ptr<BaseSectioning>> criteria;

	criteria.push_back(new_section_when_between_unique(" if (", ")", BOOL));
	criteria.push_back(new_section_when_between_unique("while ( *** ) {", "}", CURLY));


	SectionHandler handler(file, criteria);

	handler.debug_print_sections();

	std::vector<std::unique_ptr<Rule>> rules;

	//rules.push_back(new_rule_must_include_unique("=|>|<|==|>=|<=", BOOL));
	rules.push_back(new_rule_must_include_in_file_unique("WHILE|while"));
	rules.push_back(new_rule_must_include_in_file_unique("if"));

	RuleHandler ruleHandler(rules);

	if (!ruleHandler.check_rules(handler.get_compressed_sections()))
	{
		std::cerr << red_text("Rule check failed.") << std::endl;
		return 1;
	}

	IdentiferHandler identifierHandler;
	identifierHandler.add_identifier(Identifier<IdentifierType::Identify>("IF", IF));

	auto identified = identifierHandler.identify_items(file);

	for (size_t i = 0; i < identified.items.size(); i++)
	{
		std::cout << "Identified: " << identified.items[i] << " with ID: " << identified.identifiers[i] << std::endl;
	}

	std::cout << green_text("Test completed successfully.") << std::endl;

	//std::system("start cmd");

	return 0;
}