#include "headers.h"
#include "include/identifier_handler.h"


namespace parser
{
	IdentiferHandler::IdentiferHandler(std::vector<std::unique_ptr<BaseIdentifier>>& identifiers)
		: _identifier(std::move(identifiers))
	{
	}

	ItemsIdentified IdentiferHandler::identify_items(EntireTokenizedFile& tokens) const
	{
		ItemsIdentified identified;
		for (size_t i = 0; i < tokens.size(); i++)
		{
			std::string_view token = tokens[i];
			
			bool succeeded = false;

			for (const auto& identifier : _identifier)
			{
				switch (identifier->get_type())
				{
				case IdentifierType::Identify:
				{
					auto id = static_cast<Identifier<IdentifierType::Identify> const*>(identifier.get());
					if (token == id->target)
					{
						identified.items.emplace_back(token);
						identified.identifiers.push_back(id->identifier);
						succeeded = true;
					}
					break;
				}
				case IdentifierType::Ignore:
				{
					auto id = static_cast<Identifier<IdentifierType::Ignore> const*>(identifier.get());
					if (token == id->target)
					{
						i++;
						succeeded = true;
					}
					break;
				}
				case IdentifierType::IgnoreUntilFound:
				{
					auto id = static_cast<Identifier<IdentifierType::IgnoreUntilFound> const*>(identifier.get());
					if (token == id->target)
					{
						i++;
						while (i < tokens.size() && tokens[i] != id->untilFound)
						{
							i++;
						}
						succeeded = true;
					}
					break;
				}
				case IdentifierType::IdentifyIfElse:
				{
					auto id = static_cast<Identifier<IdentifierType::IdentifyIfElse> const*>(identifier.get());

					std::vector<std::string_view> tempItems;
					for (size_t j = i; j > i - id->traceBackTokens; j--)
					{
						tempItems.insert(tempItems.begin(), tokens[j]);
					}
					for (size_t j = i + 1; j < i + id->traceBackTokens && j < tokens.size(); j++)
					{
						tempItems.push_back(tokens[j]);
					}

					size_t result = id->condition(tempItems);
					if (result != std::numeric_limits<size_t>::max())
					{
						identified.items.emplace_back(token);
						identified.identifiers.push_back(result);
						succeeded = true;
					}
					break;
				}
				}

			}
	
			if (!succeeded)
			{
				identified.items.emplace_back(token);
				identified.identifiers.push_back(std::numeric_limits<size_t>::max());
			}
		
		}
		return identified;
	}

	void IdentiferHandler::add_identifier(BaseIdentifier const& identifier)
	{
		switch (identifier.get_type())
		{
		case IdentifierType::Identify:
			_identifier.push_back(std::make_unique<Identifier<IdentifierType::Identify>>(static_cast<Identifier<IdentifierType::Identify> const&>(identifier)));
			break;
		case IdentifierType::Ignore:
			_identifier.push_back(std::make_unique<Identifier<IdentifierType::Ignore>>(static_cast<Identifier<IdentifierType::Ignore> const&>(identifier)));
			break;
		case IdentifierType::IgnoreUntilFound:
			_identifier.push_back(std::make_unique<Identifier<IdentifierType::IgnoreUntilFound>>(static_cast<Identifier<IdentifierType::IgnoreUntilFound> const&>(identifier)));
			break;
		case IdentifierType::IdentifyIfElse:
			_identifier.push_back(std::make_unique<Identifier<IdentifierType::IdentifyIfElse>>(static_cast<Identifier<IdentifierType::IdentifyIfElse> const&>(identifier)));
			break;
		default:
			throw std::runtime_error("Unknown identifier type");
		}

	}

	std::vector<std::unique_ptr<BaseIdentifier>> const& IdentiferHandler::get_identifiers() const
	{
		return _identifier;
	}



}