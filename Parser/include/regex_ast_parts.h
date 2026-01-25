#pragma once


#include "config.h"


// -------------------------------------------
// ------ PARTS OF ABSTRACT SYNTAX TREE ------
// -------------------------------------------

namespace parser
{

	enum class RegType
	{
		
		// Root / Structural
		
		Base,               // Root or default node

		
		// Grouping & Containers
		
		Parentheses,        // () Capture / grouping
		Bracket,            // [] Character class

		
		// Anchors & Zero-Width Assertions
		
		StartOfString,      // ^
		EndOfString,        // $
		WordBoundary,       // \b (\bcat\b == " cat ")
		NonWordBoundary,    // \B (\Bcat\B == "cat")

		
		// Predefined Character Classes (Placeholders)
		
		NumberPH,           // \d
		NotNumberPH,        // \D
		WhitespacePH,       // \s
		NotWhitespacePH,    // \S
		DotPH,              // . (any character)
		WordPH,               // \w ([A-Za-z0-9_])
		NotWordPH,               // \W ([^A-Za-z0-9_])


		
		// Literal / Concrete Match Values
		
		Character,          // Literal character
		Number,             // Numeric literal (used in {n,m}, \1)
		
		// Comma,              // , literal (used in {n,m})

		
		// Character-Class Operators (Inside [])
		
		Dash,               // a-z range operator

		
		// Logical / Alternation Operators
		
		Or,                 // |

		
		// Quantifiers
		
		Star,               // *
		Plus,               // +
		OneOrZero,          // ?
		RepeatRangedTimes,  // {n,m} && {n,} && {,m}
		RepeatNumberedTimes,// {n} // Make this a seperate class!!!
		
		// Backreferences
		
		//Backreference,      // \1, \2, etc. -- Too Compilcated to implement right now

		// Markers (To make compiling easier)
		PerenToAddHere
	};



	class BasePart
	{
	public:

		virtual constexpr RegType type() const = 0;

		virtual bool execute(std::string_view substr, int& i) const = 0;

		virtual size_t charater_length() const = 0;

		// Figure out to execute the checks

		virtual ~BasePart() = default;
	};


	class BracketAcceptedPart
	{
	public:
		virtual size_t charater_length() const = 0;
		virtual constexpr RegType type() const = 0;
		virtual bool execute(std::string_view substr, int& i) const = 0;
		virtual ~BracketAcceptedPart() = default;

	};



	class CharacterPart : public BasePart, public BracketAcceptedPart
	{
	public:
		CharacterPart(char c, bool createdWithBackslash = false) : _char(c), _backslash(createdWithBackslash) {}

		bool execute(std::string_view substr, int& i) const override;

		size_t charater_length() const override { return 1 + (_backslash ? 1 : 0); }

		constexpr RegType type() const override { return RegType::Character; }

		~CharacterPart() = default;
	private:
		char _char;
		bool _backslash = false;

		friend class DashOp;
	};


	class DashOp : public BracketAcceptedPart
		// This is so that the bracket container can accept dashes in the inheritance tree
	{
	public:
		DashOp(char left, char right) : _left(std::make_unique<CharacterPart>(left)), _right(std::make_unique<CharacterPart>(right)) {};

		size_t charater_length() const override { return 3; }

		bool execute(std::string_view substr, int& i) const override;

		constexpr RegType type() const override { return RegType::Dash; }

		~DashOp() = default;
	private:
		std::unique_ptr<CharacterPart> _left;
		std::unique_ptr<CharacterPart> _right;
	};




	/* --- PLACEHOLDERS ---*/

	class PlaceHolder : public BasePart
	{
	public: 
		size_t charater_length() const override { return 2; }
		~PlaceHolder() = default;
	};

	class NotNumberPH : public PlaceHolder
	{
	public:
		NotNumberPH() = default;
		constexpr RegType type() const override { return RegType::NotNumberPH; }
		bool execute(std::string_view substr, int& i) const override;
		~NotNumberPH() = default;
	};

	class NumberPH : public PlaceHolder
	{
    public:
		NumberPH() = default;
		constexpr RegType type() const override { return RegType::NumberPH; }
		bool execute(std::string_view substr, int& i) const override;
		~NumberPH() = default;
	};

	class WordPH : public PlaceHolder
	{
    public:
		WordPH() = default;
		constexpr RegType type() const override { return RegType::WordPH; }
		bool execute(std::string_view substr, int& i) const override;
		~WordPH() = default;
	};

	class NotWordPH : public PlaceHolder
	{
    public:
		NotWordPH() = default;
		constexpr RegType type() const override { return RegType::NotWordPH; }
		bool execute(std::string_view substr, int& i) const override;
		~NotWordPH() = default;
	};

	class WhitespacePH : public PlaceHolder
	{
    public:
		WhitespacePH() = default;
		constexpr RegType type() const override { return RegType::WhitespacePH; }
		bool execute(std::string_view substr, int& i) const override;
		~WhitespacePH() = default;
	};

	class NotWhitespacePH : public PlaceHolder
	{
    public:
		NotWhitespacePH() = default;
		constexpr RegType type() const override { return RegType::NotWhitespacePH; }
		bool execute(std::string_view substr, int& i) const override;
		~NotWhitespacePH() = default;
	};

	class DotPH : public PlaceHolder
	{
    public:
		DotPH() = default;
		constexpr RegType type() const override { return RegType::DotPH; }
		bool execute(std::string_view substr, int& i) const override;
		~DotPH() = default;
	};


	/* --- CONTAINER REGS ---*/

	class ParenContainer : public BasePart
	{
	public:
		ParenContainer() = default;

		void add_part(std::unique_ptr<BasePart> part);

		std::unique_ptr<BasePart>& get_back_part_ref();

		void delete_back();

		void replace_back_part(std::unique_ptr<BasePart> part);

		constexpr RegType type() const override { return RegType::Parentheses; }
		
		bool execute(std::string_view substr, int& i) const override;

		size_t charater_length() const override;
		
		~ParenContainer() = default;
	private:
		std::vector<std::unique_ptr<BasePart>> _parts;
	};

	enum NegetiveFlag
	{
		NEGETIVE = 0,
		POSITIVE = 1,
	};

	class BracketContainer : public BasePart
	{
	public:
		BracketContainer() = default;
		BracketContainer(NegetiveFlag negetive) : _not(negetive) {}

		constexpr RegType type() const override { return RegType::Bracket; }

		bool execute(std::string_view substr, int& i) const override;

		size_t charater_length() const override;

		void add_char(char part, bool usesBackslash = false);
		void add_dash(char left, char right);
		
		~BracketContainer() override = default;
	private:
		NegetiveFlag _not = POSITIVE;
		std::vector<std::unique_ptr<BracketAcceptedPart>> _parts;
	};


	class RepeatPartsBase : public BasePart
	{
	public:
		RepeatPartsBase() = default;

		void add(std::unique_ptr<BasePart> part);

		size_t charater_length() const override;

		~RepeatPartsBase() = default;
	protected:
		std::unique_ptr<BasePart> _repetition;
	};


	// This is a quantifier container
	class RepeatRangedTimes : public RepeatPartsBase
	{
	public:
		RepeatRangedTimes(std::optional<uint16_t> min, std::optional<uint16_t> max);

		constexpr RegType type() const override { return RegType::RepeatRangedTimes; }

		bool execute(std::string_view substr, int& i) const override;
		
		size_t charater_length() const override;

		~RepeatRangedTimes() override = default;
	private:
			std::optional<uint16_t> _min = std::nullopt;
			std::optional<uint16_t> _max = std::nullopt;

	};

	class RepeatNumberedTimes : public RepeatPartsBase
	{
	public:
		RepeatNumberedTimes(uint16_t times);

		constexpr RegType type() const override { return RegType::RepeatNumberedTimes; }

		bool execute(std::string_view substr, int& i) const override;

		~RepeatNumberedTimes() override = default;
	private:
		uint16_t _times;
	};

	class OrOp : public BasePart
	{
	public:
		OrOp(std::unique_ptr<BasePart> left) { _left = std::move(left); };

		size_t charater_length() const override;
		
		bool execute(std::string_view substr, int& i) const override;

		void set_right(std::unique_ptr<BasePart> part);

		constexpr RegType type() const override { return RegType::Or; }

		~OrOp() override = default;
	private:
			std::unique_ptr<BasePart> _left;
			std::unique_ptr<BasePart> _right;
	};






	class StarOp : public RepeatPartsBase
	{
	public:

		StarOp() = default;

		constexpr RegType type() const override { return RegType::Star; }

		bool execute(std::string_view substr, int& i) const override;

		~StarOp() override = default;
	};


	class PlusOp : public RepeatPartsBase // One or more times
	{
	public:
		PlusOp() = default;

		bool execute(std::string_view substr, int& i) const override;

		constexpr RegType type() const override { return RegType::Plus; }

		~PlusOp() override = default;

	};


	class OneOrZeroOp : public RepeatPartsBase
	{
    public:
		OneOrZeroOp() = default;
		constexpr RegType type() const override { return RegType::OneOrZero; }
		bool execute(std::string_view substr, int& i) const override;
		~OneOrZeroOp() override = default;
	};


	class StringPlacementPartsBase : public BasePart
	{
	public:
		StringPlacementPartsBase() = default;
		bool execute(std::string_view substr, int& i) const override { return true; };
		virtual ~StringPlacementPartsBase() = default;
	protected:
	};

	class StartOfString : public StringPlacementPartsBase
	{
	public:
		StartOfString() = default;

		constexpr RegType type() const override { return RegType::StartOfString; }

		size_t charater_length() const override;

		~StartOfString() override = default;
	private:
	};

	class EndOfString : public StringPlacementPartsBase
	{
	public:
		EndOfString() = default;

		constexpr RegType type() const override { return RegType::EndOfString; }

		size_t charater_length() const override;

		~EndOfString() override = default;
	private:
	};

	class WordBoundary : public StringPlacementPartsBase
	{
	public:
		WordBoundary() = default;

		bool execute(std::string_view substr, int& i) const override;

		size_t charater_length() const override;

		constexpr RegType type() const override { return RegType::WordBoundary; }

		~WordBoundary() override = default;

	};

	class NonWordBoundary : public StringPlacementPartsBase
	{
	public:
		NonWordBoundary() = default;

		bool execute(std::string_view substr, int& i) const override;

		size_t charater_length() const override;

		constexpr RegType type() const override { return RegType::NonWordBoundary; }

		~NonWordBoundary() override = default;
	private:
	};

	//using CharOrNumber = std::variant<CharacterPart, NumberPart>;

	class PerenMarker : public BasePart
	{
	public:
		PerenMarker() = default;
		bool execute(std::string_view substr, int& i) const override { return true; }
		constexpr RegType type() const override { return RegType::PerenToAddHere; }
		size_t charater_length() const override { return 0; }
		~PerenMarker() override = default;
	};
}
