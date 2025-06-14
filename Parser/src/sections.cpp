#include "headers.h"

#include "include/sections.h"


namespace parser
{




	std::unique_ptr<Sectioning<2, HasIdentifier::Yes>>  new_section_when_before_unique(std::string const& t1, std::string const& t2, size_t id)
	{
		return make_sectioning_unique<2, HasIdentifier::Yes>({ t1, t2 }, id, ParserSectioning::NewSectionWhenBefore);
	}

	std::shared_ptr<Sectioning<2, HasIdentifier::Yes>> new_section_when_before_shared(std::string const& t1, std::string const& t2, size_t id)
	{
		return make_sectioning_shared<2, HasIdentifier::Yes>({ t1, t2 }, id, ParserSectioning::NewSectionWhenBefore);
	}

	Sectioning<1, HasIdentifier::Yes> new_section_when_found(std::string const& target, size_t id)
	{
		return make_sectioning_value<1, HasIdentifier::Yes>({ target }, id, ParserSectioning::NewSectionWhenFound);
	}

	std::shared_ptr<Sectioning<1, HasIdentifier::Yes>> new_section_when_found_shared(std::string const& target, size_t id)
	{
		return make_sectioning_shared<1, HasIdentifier::Yes>({ target }, id, ParserSectioning::NewSectionWhenFound);
	}

	std::unique_ptr<Sectioning<1, HasIdentifier::Yes>>  new_section_when_found_unique(std::string const& target, size_t id)
	{
		return make_sectioning_unique<1, HasIdentifier::Yes>({ target }, id, ParserSectioning::NewSectionWhenFound);
	}

	Sectioning<2, HasIdentifier::Yes> new_section_when_between(std::string const& t1, std::string const& t2, size_t id)
	{
		return make_sectioning_value<2, HasIdentifier::Yes>({ t1, t2 }, id, ParserSectioning::NewSectionWhenBetween);
	}

	std::shared_ptr<Sectioning<2, HasIdentifier::Yes>> new_section_when_between_shared(std::string const& t1, std::string const& t2, size_t id)
	{
		return make_sectioning_shared<2, HasIdentifier::Yes>({ t1, t2 }, id, ParserSectioning::NewSectionWhenBetween);
	}

	std::unique_ptr<Sectioning<2, HasIdentifier::Yes>>  new_section_when_between_unique(std::string const& t1, std::string const& t2, size_t id)
	{
		return make_sectioning_unique<2, HasIdentifier::Yes>({ t1, t2 }, id, ParserSectioning::NewSectionWhenBetween);
	}

	Sectioning<2, HasIdentifier::Yes> new_section_when_after(std::string const& t1, std::string const& t2, size_t id)
	{
		return make_sectioning_value<2, HasIdentifier::Yes>({ t1, t2 }, id, ParserSectioning::NewSectionWhenAfter);
	}

	std::shared_ptr<Sectioning<2, HasIdentifier::Yes>> new_section_when_after_shared(std::string const& t1, std::string const& t2, size_t id)
	{
		return make_sectioning_shared<2, HasIdentifier::Yes>({ t1, t2 }, id, ParserSectioning::NewSectionWhenAfter);
	}

	std::unique_ptr<Sectioning<2, HasIdentifier::Yes>>  new_section_when_after_unique(std::string const& t1, std::string const& t2, size_t id)
	{
		return make_sectioning_unique<2, HasIdentifier::Yes>({ t1, t2 }, id, ParserSectioning::NewSectionWhenAfter);
	}

	Sectioning<2, HasIdentifier::Yes> new_section_when_before(std::string const& t1, std::string const& t2, size_t id)
	{
		return make_sectioning_value<2, HasIdentifier::Yes>({ t1, t2 }, id, ParserSectioning::NewSectionWhenBefore);
	}


}

