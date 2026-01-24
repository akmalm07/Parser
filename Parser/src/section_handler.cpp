#include "headers.h"
#include "include/section_handler.h"
#include "include/parser.h"
#include "include/tokenizer.h"



// -------------------------------------------
// ----- REPLACE WITH REGEX SYSTEM LATER -----
// -------------------------------------------

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

						for (size_t j = 0; j < placementNum; j++)
						{
							content.push_back(*(placement + j));
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
						std::vector<std::string> stopTrigger;

						if (criteria->is_identifible())
						{
							auto target = static_cast<SectioningIdentifier<2>*>(criteria)->targets[1];

							if (str_includes(target, " "))
							{
								auto parts = tokenize(target, TokenizationSeperationBitFlags::TockeizeSpace, WhiteSpaceDissolveBitFlags::DissolveAll);
								stopTrigger.insert(stopTrigger.end(), parts.begin(), parts.end());
							}
							else
							{
								stopTrigger.push_back(target);
							}
						}
						else
						{
							auto target = static_cast<SectioningUnidentifier<2>*>(criteria)->targets[1];

							if (str_includes(target, " "))
							{
								auto parts = tokenize(target, TokenizationSeperationBitFlags::TockeizeSpace, WhiteSpaceDissolveBitFlags::DissolveAll);
								stopTrigger.insert(stopTrigger.end(), parts.begin(), parts.end());
							}
							else
							{
								stopTrigger.push_back(target);
							}
						}

				//Critical Change
						for (size_t j = 0; j < placementNum; j++)
						{
							for (size_t k = 0; k < stopTrigger.size(); k++)
							{
								if (stopTrigger[k] == *(placement + j + k) || (placement + j + k) == currentEndOfSection)
								{
									content.push_back(*(placement + j));
									currentEndOfSection = placement + j;
									break;
								}
								content.push_back(*(placement + j));
							}
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
						for (size_t j = 0; j < placementNum; j++)
						{
							if (stopTrigger == *(placement + j))
							{
								break;
							}
							content.push_back(*(placement + j));
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

		auto update = [&](TockenizedUnsectionedFileIteratorConst iterator, size_t iterationForExecutor)
			{	

				auto result = userCriteria.execute[iterationForExecutor]->execute(SectioningInput(iterator, currentEndOfSection, sectionAbove));

				const auto beginningIterator = iterator + 1;

				_sectionKeys.emplace_back(result.section->get_section_level(), result.section->get_section_identifier(), _sectionValues.size() - 1,
					SectionCoords(beginningIterator - file.begin(), result.endOfSection - file.begin()));

				_compressedSections.coords.emplace_back(beginningIterator - file.begin(), result.endOfSection - file.begin(), result.section->get_section_identifier());

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

					bool noReg = true;

					std::string str(trigger[0]);
					auto parts = tokenize(str, TokenizationSeperationBitFlags::TockeizeSpace, WhiteSpaceDissolveBitFlags::DissolveSpace);
				
					if (str_includes(trigger[0], " "))
					{

						

						
						bool findUntilSuccess = check_find_until_regex(str, file, i);

						bool skipOneSuccess = check_skip_one_regex(str, file, i);




							/*noReg = false;
							auto check1 = std::find(parts.begin(), parts.end(), *(orBeginReg + 1));
							auto check2 = std::find(parts.begin(), parts.end(), *(orSeporatorReg + 1));
							auto check3 = std::find(parts.begin(), parts.end(), *(orEndReg + 1));
							if (check1 != parts.end() && check2 != parts.end() && check3 != parts.end())
							{
								if (file[i] == *check1 || file[i] == *check2 || file[i] == *check3)
								{
									triggers.insert(triggers.end(), parts.begin(), orBeginReg);
									regSuccess = true;
								}
							}*/
						

						
						//if (!regSuccess)
							//continue;
					}

					std::vector<std::string> triggers;
					if (noReg)
					{
						triggers.emplace_back(trigger[0]);
						triggers.insert(triggers.end(), parts.begin(), parts.end());
					}

					//	stopTrigger.insert(stopTrigger.end(), std::make_move_iterator(parts.rbegin()), std::make_move_iterator(parts.rend()));
					
					bool success = true;
					if(noReg)
					{

						for (size_t k = 0; k < triggers.size(); k++)
						{
							if (triggers[k] != file[i + k] || i + k >= file.size())
							{
								success = false;
								break;
							}
						}
					}


					
					if (success)
						update(it, j);

					break;
				}
	
			}
		}
	}

	bool SectionHandler::check_find_until_regex(std::string& trigger, TockenizedUnsectionedFile const& file, size_t currentIndex) const
	{
		auto parts = tokenize(trigger, TokenizationSeperationBitFlags::TockeizeSpace, WhiteSpaceDissolveBitFlags::DissolveSpace);
		auto findUntilReg = std::find(parts.begin(), parts.end(), REGEX_KEYWORD_LIST[0]);
		if (findUntilReg != parts.end())
		{
			for (size_t k = currentIndex; k < file.size(); k++)
			{
				if (file[k] == *(findUntilReg + 1))
				{
					return true;
				}
			}
		}
		return false;
	}

	bool SectionHandler::check_skip_one_regex(std::string& trigger, TockenizedUnsectionedFile const& file, size_t currentIndex) const
	{
		auto parts = tokenize(trigger, TokenizationSeperationBitFlags::TockeizeSpace, WhiteSpaceDissolveBitFlags::DissolveSpace);
		auto findUntilReg = std::find(parts.begin(), parts.end(), REGEX_KEYWORD_LIST[1]);
		if (findUntilReg != parts.end())
		{
			if (currentIndex + 1 < file.size() && file[currentIndex + 1] == *(findUntilReg + 1))
			{
				return true;
			}
		}
		return false;
	}

	bool SectionHandler::check_or_regex(std::string& trigger, TockenizedUnsectionedFile const& file, size_t currentIndex) const
	{
		auto parts = tokenize(trigger, TokenizationSeperationBitFlags::TockeizeSpace, WhiteSpaceDissolveBitFlags::DissolveSpace);

		auto orBeginReg = std::find(parts.begin(), parts.end(), REGEX_KEYWORD_LIST[2]);
		auto orSeporatorReg = std::find(parts.begin(), parts.end(), REGEX_KEYWORD_LIST[3]);
		auto orEndReg = std::find(parts.begin(), parts.end(), REGEX_KEYWORD_LIST[4]);

		if (orBeginReg != parts.end() && orSeporatorReg != parts.end() && orEndReg != parts.end()
			&& orBeginReg < orSeporatorReg && orSeporatorReg < orEndReg)
		{
			size_t orCount = 0;
			for (auto iterator = orSeporatorReg; iterator < orEndReg; iterator++)
			{
				if (*iterator == REGEX_KEYWORD_LIST[3])
					orCount++;
			}

			std::vector<std::string_view> options;
			options.reserve(orCount + 1);

			std::string accumulated;
			for (auto iterator = orBeginReg + 1; iterator <= orEndReg; iterator++)
			{
				if (*iterator != REGEX_KEYWORD_LIST[3] || *iterator != REGEX_KEYWORD_LIST[3])
				{
					options.push_back(accumulated);
					accumulated = "";
				}
				else
				{
					if (accumulated == REGEX_KEYWORD_LIST[0] || accumulated == REGEX_KEYWORD_LIST[1])
						accumulated += " ";
					accumulated += *iterator;
				}
			}

			for (auto const& opt : options)
			{
				if (str_includes(opt, " "))
				{
					if (check_skip_one_regex(trigger, file, currentIndex) || check_find_until_regex(trigger, file, currentIndex))
					{
						return true;
					}
				}
				else if (file[currentIndex] == opt) 
				{
					return true;
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

