#include "include/tockenization.h"


namespace parser
{

	std::vector<std::string> tokenize(std::string_view text)
	{

		std::istringstream stream{ std::string(text) };
		std::vector<std::string> tokens;
		std::string word;

		while (stream >> word)
		{
			tokens.push_back(word);
		}

		return tokens;
	}


    bool check_str(std::string_view str, StringChecksFlag flags)
    {
        if (str.empty())
        {
            return false;
        }

        for (char c : str)
        {
            if (!(flags & StringChecksBitFlags::NoDigitCheck) && isdigit(static_cast<unsigned char>(c)))
            {
                return false;
            }

            if (!(flags & StringChecksBitFlags::NoSpecialCharCheck) && !isalnum(static_cast<unsigned char>(c)))
            {
                return false;
            }

            if (!(flags & StringChecksBitFlags::NoAtoZCheck) && isalpha(static_cast<unsigned char>(c)))
            {
                return false;
            }

            if (!(flags & StringChecksBitFlags::NoSpaceCheck) && isspace(static_cast<unsigned char>(c)))
            {
                return false;
            }

            if (!(flags & StringChecksBitFlags::NoNewLineCheck) && c == '\n')
            {
                return false;
            }

            if (!(flags & StringChecksBitFlags::NoTabCheck) && c == '\t')
            {
                return false;
            }
        }

        return true;
    }



	std::string print_tokens(TockenizedSection const& tokens)
	{
		std::ostringstream oss;
		for (const auto& token : tokens)
		{
			oss << token << " ";
		}
		return oss.str();
	}




}