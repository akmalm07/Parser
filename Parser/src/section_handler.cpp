#include "headers.h"
#include "include/section_handler.h"


namespace parser {

	void SectionHandler::section_tokens(TockenizedSections const& tokenizedSections, std::vector<BaseSectioning> const& criteria)
	{
		// This function is responsible for processing the tokenized sections and applying the sectioning criteria.
		// It will create sections based on the criteria provided and organize the tokens accordingly.

		std::vector<std::pair<BaseSectioning const&, std::function<std::shared_ptr<Section>(TockenizedSectionsIterator)>>> lookOutFor;

		lookOutFor.reserve(criteria.size());

		for (const auto& section : tokenizedSections)
		{
			// Process each section based on the criteria
			for (const auto& criterion : criteria)
			{
				if (criterion.type == ParserSectioning::NewSectionWhenFound)
				{ 
					lookOutFor.emplace_back(std::make_pair(criterion, [&](TockenizedSectionsIterator it) -> std::shared_ptr<Section>
						{
							std::vector<std::string_view> content;
							content.reserve(tokenizedSections.end() - it);
							while (it != tokenizedSections.end())
							{
								
							}

							return std::make_shared<Section>();
						}
					));
				}
				
			}
		}
	}

}
