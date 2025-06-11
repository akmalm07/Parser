#pragma once

#include "include/config.h"
#include "include/rules.h"
#include "include/tokenizer.h"


namespace parser {


	//System Implementation:
	/*
	* The RuleHandler system is designed to handle various rules that can be applied to sections of a file.
	* The system uses polymorphism to allow different types of rules to be checked against the data.
	* The RuleInput is a type alias that can hold either a single TokenizedSection or multiple TokenizedSections.
	* This is to ensure that the RuleHandler can handle both global rules that apply to multiple sections and local rules that apply to a single section.
	* The check_rule method in the RuleHandler class is a virtual method that takes a RuleInput and checks if the rule applies to the data.
	* The method is expectantly useful for global rules that need to check multiple sections at once, and allows Local rules to check single sections.
	*/
	

	//TESTING STATUS: Incomplete



}