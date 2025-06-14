#include "headers.h"
#include "include/section_handler.h"


namespace parser {
	
	template <size_t T, bool Both>
	void add_triggers(std::unique_ptr<BaseSectioning> const& criteria, std::vector<std::array<std::string_view, 2>>& triggers);


	void SectionHandler::add_section(SectionKey const& key, std::unique_ptr<BaseSection>&& section)
	{
		_sectionValues.push_back(std::move(section));
	}


	void SectionHandler::debug_print_sections() const
	{
		for (const auto& section : _sectionValues)
		{
			std::cout << "Section Level: " << section->get_section_level() << ", Content: ";
			for (const auto& token : section->get_content())
			{
				std::cout << token << " ";
			}
			std::cout << std::endl;
		}
	}

	TokenizedSectionizedCompact SectionHandler::get_compressed_sections() const
	{
		return _compressedSections;
	}

	void SectionHandler::remove_section(SectionKey const& key)
	{
		_sectionKeys.erase(
			std::remove(_sectionKeys.begin(), _sectionKeys.end(), key),
			_sectionKeys.end());
	}

	CriteriaProcesserOutput SectionHandler::user_criteria_processer(TockenizedUnsectionedFile const& file, std::vector<std::unique_ptr<BaseSectioning>> const& criteria)
	{
		// This function is responsible for processing the tokenized sections and applying the sectioning criteria.
		// It will create sections based on the criteria provided and organize the tokens accordingly.


		std::vector<ParserSectioning> sectioningType;

		std::vector<std::array<std::string_view, 2>> triggers;

		std::vector<std::unique_ptr<ExecuteFunctorBase>> execute;

		execute.reserve(criteria.size());
		triggers.reserve(criteria.size());


		for (size_t i = 0; i < criteria.size(); i++)
		{
			switch(criteria[i]->type)
			{ 
			case ParserSectioning::NewSectionWhenFound:
				sectioningType.emplace_back(ParserSectioning::NewSectionWhenFound);
				if (criteria[i]->get_target_count() != 1)
				{
					std::cerr << PARSER_LOG_ERR << "Input for BaseSectioning Must have at only one target." << std::endl;
					return CriteriaProcesserOutput();
				}

				add_triggers<1, false>(criteria[i], triggers);

				execute.emplace_back(std::make_unique<ExecuteFunc<1>>(criteria[i].get(), []
				(size_t placementNum, TockenizedUnsectionedFileIteratorConst placement, TockenizedUnsectionedFileIteratorConst currentEndOfSection) -> ExecutionOutput
					{
						std::vector<std::string_view> content;
						content.reserve(placementNum);

						for (size_t i = 0; i < placementNum; i++)
						{
							content.push_back(*(placement + i));
						}

						return { currentEndOfSection, content };
					})
				);
				break;
			
			case ParserSectioning::NewSectionWhenBetween:

				sectioningType.emplace_back(ParserSectioning::NewSectionWhenBetween);
				if (criteria[i]->get_target_count() != 2)
				{
					std::cerr << PARSER_LOG_ERR << "Input for BaseSectioning Must have at only two targets." << std::endl;
					return CriteriaProcesserOutput();
				}

				add_triggers<2, false>(criteria[i], triggers);

				execute.emplace_back(std::make_unique<ExecuteFuncWithCriteria<2>>(criteria[i].get(), []
					(size_t placementNum, TockenizedUnsectionedFileIteratorConst placement, TockenizedUnsectionedFileIteratorConst currentEndOfSection, BaseSectioning* criteria) -> ExecutionOutput
					{
						std::vector<std::string_view> content;
						content.reserve(placementNum);
						std::string_view stopTrigger;
						if (criteria->is_identifible())
						{
							stopTrigger = static_cast<SectioningIdentifier<2>*>(criteria)->targets[1];
						}
						else
						{
							stopTrigger = static_cast<SectioningUnidentifier<2>*>(criteria)->targets[1];
						}

						if (stopTrigger == *placement)
						{
							return { currentEndOfSection, content};
						}
						for (size_t i = 1; i < placementNum; i++)
						{
							if (stopTrigger == *(placement + i) || (placement + i) == currentEndOfSection)
							{
								currentEndOfSection = placement + i;
								break;
							}
							content.push_back(*(placement + i));
						}

						return { currentEndOfSection, content };
					})
				);
				break;
			
			case ParserSectioning::NewSectionWhenAfter: 
			case ParserSectioning::NewSectionWhenBefore:

				if (criteria[i]->get_target_count() != 2)
				{
					std::cerr << PARSER_LOG_ERR << "Input for BaseSectioning Must have at only two targets." << std::endl;
					return CriteriaProcesserOutput();
				}
			
				if (criteria[i]->type == ParserSectioning::NewSectionWhenAfter)
				{
					sectioningType.emplace_back(ParserSectioning::NewSectionWhenAfter);
				}
				else
				{
					sectioningType.emplace_back(ParserSectioning::NewSectionWhenBefore);
				}

				add_triggers<2, true>(criteria[i], triggers);

				execute.emplace_back(std::make_unique<ExecuteFuncWithCriteria<2>>(criteria[i].get(), []
					(size_t placementNum, TockenizedUnsectionedFileIteratorConst placement, TockenizedUnsectionedFileIteratorConst currentEndOfSection, BaseSectioning* criteria) -> ExecutionOutput
					{
						std::vector<std::string_view> content;
						content.reserve(placementNum);
						std::string_view stopTrigger;
						if (criteria->is_identifible())
						{
							stopTrigger = static_cast<SectioningIdentifier<2>*>(criteria)->targets[1];
						}
						else
						{
							stopTrigger = static_cast<SectioningUnidentifier<2>*>(criteria)->targets[1];
						}
						for (size_t i = 0; i < placementNum; i++)
						{
							if (stopTrigger == *(placement + i))
							{
								break;
							}
							content.push_back(*(placement + i));
						}
						return { currentEndOfSection, content };
					}) 
				);
			
				break;
			default:
				std::cerr << PARSER_LOG_ERR << "Unknown sectioning type." << std::endl;
				return CriteriaProcesserOutput();
			}
		}

		if (triggers.empty() || ( execute.size() != triggers.size() && triggers.size() != sectioningType.size() ) )
		{
			std::cerr << PARSER_LOG_ERR << "No valid sectioning criteria provided." << std::endl;
			return CriteriaProcesserOutput();
		}

		return CriteriaProcesserOutput{ sectioningType, triggers, execute };
	}



	void SectionHandler::process_sectioning(TockenizedUnsectionedFile const& file, std::vector<std::unique_ptr<BaseSectioning>> const& criteria)
	{
		CriteriaProcesserOutput userCriteria = user_criteria_processer(file, criteria);



		_compressedSections.coords.reserve(file.size() / 4);
		_compressedSections.tokens = file;

		_sectionKeys.reserve(file.size() / 6);

		_sectionValues.reserve(file.size() / 6);


		_sectionValues.emplace_back(std::make_unique<BaseSection>(1, file, nullptr));
		_sectionKeys.emplace_back(0, 0, 0, SectionCoords(0, file.size()));
		view_ptr_non_const<BaseSection> sectionAbove = _sectionValues.back().get();


		TockenizedUnsectionedFileIteratorConst currentEndOfSection = file.end();
		std::vector<TockenizedUnsectionedFileIteratorConst> prevEndOfSections;

		auto update = [&](TockenizedUnsectionedFileIteratorConst iterator, size_t iteration)
			{
				
				DEBUG(
					std::cout << "Value for the currentEndOfSection:   " << (currentEndOfSection == file.end() ? "NULL" : *currentEndOfSection) << "   :and: " << *iterator << std::endl; // ERR, cannot dereference (*) the end of the file! its null
					)

				auto result = userCriteria.execute[iteration]->execute(SectioningInput(iterator, currentEndOfSection, sectionAbove));

				_sectionKeys.emplace_back(result.section->get_section_level(), result.section->get_section_identifier(), _sectionValues.size() - 1,
					SectionCoords(iterator - file.begin(), result.endOfSection - file.begin()));

				_compressedSections.coords.emplace_back(iterator - file.begin(), result.endOfSection - file.begin(), result.section->get_section_identifier());

				prevEndOfSections.push_back(currentEndOfSection);
				currentEndOfSection = result.endOfSection;

				_sectionValues.emplace_back(std::move(result.section));
				
				sectionAbove = _sectionValues.back().get();
				DEBUG(std::cout << "Section above updated to: " << sectionAbove->get_section_level() << std::endl;);
			};

		for (size_t i = 0; i < file.size(); i++)
		{
			TockenizedUnsectionedFileIteratorConst it = file.begin() + i;

			for (auto const& [j, trigger] : userCriteria.triggers | std::views::enumerate)
			{

				if (currentEndOfSection != file.end() && file[i] == *currentEndOfSection)
				{
					currentEndOfSection = prevEndOfSections.empty() ? file.end() : prevEndOfSections.back();
					prevEndOfSections.pop_back();
					
					DEBUG(std::cout << "CurrentEndOfSection updated to: " << (currentEndOfSection == file.end() ? "NULL" : *currentEndOfSection) << std::endl;);
					DEBUG(std::cout << "Section above updated to: " << sectionAbove->get_section_level() << std::endl;);
					
					sectionAbove = sectionAbove->get_section_above();
				}

				switch (userCriteria.sectioningType[j])
				{
				case ParserSectioning::NewSectionWhenFound:

					DEBUG(
						std::cout << "NewSectionWhenFound Trigger: " << trigger[0] << " and " << file[i] << std::endl;
					)
					if (trigger[0] == file[i])
					{
						update(it, j);
					}
					break;
				case ParserSectioning::NewSectionWhenAfter:

					if (i > 0 && trigger[0] == file[i] && trigger[1] == file[i - 1])
					{
						update(it, j);

					}
					break;
				case ParserSectioning::NewSectionWhenBefore:

					if (i + 1 < file.size() && trigger[0] == file[i] && trigger[1] == file[i + 1])
					{
						update(it, j);

					}
					break;
				case ParserSectioning::NewSectionWhenBetween:

					DEBUG(std::cout << "NewSectionWhenBetween Trigger: " << trigger[0] << " and " << file[i] << std::endl;)

					if (trigger[0] == file[i])
					{
						update(it, j);

					}
					break;
				}
	
			}
		}
	}






	template<size_t T, bool Both>
	void add_triggers(std::unique_ptr<BaseSectioning> const& criteria, std::vector<std::array<std::string_view, 2>>& triggers)
	{
		if (criteria->is_identifible())
		{
			if constexpr (T == 1)
			{
				if constexpr (Both)
					triggers.push_back({ static_cast<SectioningIdentifier<T>*>(criteria.get())->target, static_cast<SectioningIdentifier<T>*>(criteria.get())->target });
				else
					triggers.push_back({ static_cast<SectioningIdentifier<T>*>(criteria.get())->target, "" });
			}
			else
			{
				if constexpr (Both)
					triggers.push_back({ static_cast<SectioningIdentifier<T>*>(criteria.get())->targets[0], static_cast<SectioningIdentifier<T>*>(criteria.get())->targets[1] });
				else
					triggers.push_back({ static_cast<SectioningIdentifier<T>*>(criteria.get())->targets[0], "" });
			}
		}
		else
		{
			if constexpr (T == 1)
			{
				if constexpr (Both)
					triggers.push_back({ static_cast<SectioningUnidentifier<T>*>(criteria.get())->target, static_cast<SectioningIdentifier<T>*>(criteria.get())->target });
				else
					triggers.push_back({ static_cast<SectioningUnidentifier<T>*>(criteria.get())->target, ""});
			}
			else
			{
				if constexpr (Both)
					triggers.push_back({ static_cast<SectioningUnidentifier<T>*>(criteria.get())->targets[0], static_cast<SectioningIdentifier<T>*>(criteria.get())->targets[1] });
				else
					triggers.push_back({ static_cast<SectioningUnidentifier<T>*>(criteria.get())->targets[0], "" });
			}
		}
	}

}

