#include "headers.h"
#include "include/section_handler.h"


namespace parser {
	
	template <size_t T, bool Both>
	void add_triggers(std::unique_ptr<BaseSectioning> const& criteria, std::vector<std::array<std::string_view, 2>>& triggers);


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

				execute.emplace_back(std::make_unique<ExecuteFunc<1>>(criteria[i], []
				(size_t placementNum, TockenizedUnsectionedFileIteratorConst placement, TockenizedUnsectionedFileIteratorConst endOfSection) -> ExecutionOutput
					{
						std::vector<std::string_view> content;
						content.reserve(placementNum);

						for (size_t i = 0; i < placementNum; i++)
						{
							content.push_back(*(placement + i));
						}

						return { endOfSection, content };
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

				execute.emplace_back(std::make_unique<ExecuteFuncTwoIterators<2>>(criteria[i], []
					(size_t placementNum, TockenizedUnsectionedFileIteratorConst placement, TockenizedUnsectionedFileIterator endOfSection, BaseSectioning* criteria) -> ExecutionOutput
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
							if (stopTrigger == *(placement + i) || placement == endOfSection)
							{
								break;
							}
							content.push_back(*(placement + i));
						}

						return content;
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

				execute.emplace_back(std::make_unique<ExecuteFuncOneIterator<2>>(criteria[i],
					[]
					(size_t placementNum, TockenizedUnsectionedFileIteratorConst placement, BaseSectioning* criteria) -> ExecutionOutput
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
					}
					));
			
				break;
			default:
				std::cerr << PARSER_LOG_ERR << "Unknown sectioning type." << std::endl;
				return CriteriaProcesserOutput();
			}
		}

		if (triggers.empty() || (execute.size() != triggers.size() != sectioningType.size()))
		{
			std::cerr << PARSER_LOG_ERR << "No valid sectioning criteria provided." << std::endl;
			return CriteriaProcesserOutput();
		}

		return CriteriaProcesserOutput{ sectioningType, triggers, execute };
	}



	void SectionHandler::process_sectioning(TockenizedUnsectionedFile const& file, std::vector<std::unique_ptr<BaseSectioning>> const& criteria)
	{
		CriteriaProcesserOutput output = user_criteria_processer(file, criteria);

		std::shared_ptr<BaseSection> sectionAbove = std::make_shared<BaseSection>(nullptr, 0);

		TockenizedUnsectionedFileIteratorConst endOfSection = file.end();


		auto update = [&](TockenizedUnsectionedFileIteratorConst iterator, size_t iteration)
			{
				auto result = output.execute[iteration]->execute(SectioningInput(iterator, endOfSection, sectionAbove));
				_sectionValues.emplace_back(result.section);
				endOfSection = result.endOfSection;
				sectionAbove = _sectionValues.back();
			};

		for (size_t i = 0; i < file.size(); i++)
		{
			TockenizedUnsectionedFileIteratorConst it = file.begin() + i;

			for (size_t j = 0; j < output.triggers.size(); j++)
			{
				switch (output.sectioningType[j])
				{
				case ParserSectioning::NewSectionWhenFound:
					
					if (output.triggers[j][0] == file[i])
					{
						update(it, j);
					}
					break;
				case ParserSectioning::NewSectionWhenAfter: 

					if (output.triggers[j][0] == file[i] && output.triggers[j][0] == file[i - 1])
					{
						update(it, j);

					}
					break;
				case ParserSectioning::NewSectionWhenBefore:

					if (it != file.end() && output.triggers[j][0] == file[i] && output.triggers[j][0] == file[i + 1])
					{
						update(it, j);

					}
					break;
				case ParserSectioning::NewSectionWhenBetween:

					if (output.triggers[j][0] == file[i])
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
					triggers.push_back({ static_cast<SectioningIdentifier<T>*>(criteria.get())->target });
			}
			else
			{
				if constexpr (Both)
					triggers.push_back({ static_cast<SectioningIdentifier<T>*>(criteria.get())->targets[0], static_cast<SectioningIdentifier<T>*>(criteria.get())->targets[1] });
				else
					triggers.push_back({ static_cast<SectioningIdentifier<T>*>(criteria.get())->targets[0] });
			}
		}
		else
		{
			if constexpr (T == 1)
			{
				if constexpr (Both)
					triggers.push_back({ static_cast<SectioningUnidentifier<T>*>(criteria.get())->target, static_cast<SectioningIdentifier<T>*>(criteria.get())->target });
				else
					triggers.push_back({ static_cast<SectioningUnidentifier<T>*>(criteria.get())->target });
			}
			else
			{
				if constexpr (Both)
					triggers.push_back({ static_cast<SectioningUnidentifier<T>*>(criteria.get())->targets[0], static_cast<SectioningIdentifier<T>*>(criteria.get())->targets[1] });
				else
					triggers.push_back({ static_cast<SectioningUnidentifier<T>*>(criteria.get())->targets[0] });
			}
		}
	}

}

