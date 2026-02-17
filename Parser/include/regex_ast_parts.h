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
		
		Backreference,      // \1, \2, etc. -- Too Compilcated to implement right now

		// Markers (To make compiling easier)
		PerenToAddHere
	};



	class BasePart
	{
	public:

		virtual constexpr RegType type() const = 0;

		virtual bool execute(std::string_view substr, size_t& i) = 0;

		virtual size_t charater_length() const = 0;

		// Figure out to execute the checks

		std::string get_matched_string() const { return _matchedString; }

		virtual std::string print() const;

		virtual ~BasePart() = default;
	protected:
		std::string _matchedString = "";

	};


	class BracketAcceptedPart
	{
	public:
		virtual size_t charater_length() const = 0;
		virtual constexpr RegType type() const = 0;
		virtual std::string print() const = 0;
		virtual bool execute(std::string_view substr, size_t& i)  = 0;
		virtual ~BracketAcceptedPart() = default;

	};



	class CharacterPart : public BasePart, public BracketAcceptedPart
	{
	public:
		CharacterPart(char c, bool createdWithBackslash = false);

		bool execute(std::string_view substr, size_t& i)  override;

		size_t charater_length() const override { return 1 + (_backslash ? 1 : 0); }

		constexpr RegType type() const override { return RegType::Character; }

		std::string print() const override;

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

		std::string get_matched_string() const;

		bool execute(std::string_view substr, size_t& i)  override;

		std::string print() const override;

		constexpr RegType type() const override { return RegType::Dash; }

		~DashOp() = default;
	private:
		std::unique_ptr<CharacterPart> _left;
		std::unique_ptr<CharacterPart> _right;

		std::string _matchedString = "";
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
		std::string print() const override;
		bool execute(std::string_view substr, size_t& i)  override;
		~NotNumberPH() = default;
	};

	class NumberPH : public PlaceHolder
	{
    public:
		NumberPH() = default;
		constexpr RegType type() const override { return RegType::NumberPH; }
		std::string print() const override;
		bool execute(std::string_view substr, size_t& i)  override;
		~NumberPH() = default;
	};

	class WordPH : public PlaceHolder
	{
    public:
		WordPH() = default;
		constexpr RegType type() const override { return RegType::WordPH; }
		std::string print() const override;
		bool execute(std::string_view substr, size_t& i)  override;
		~WordPH() = default;
	};

	class NotWordPH : public PlaceHolder
	{
    public:
		NotWordPH() = default;
		constexpr RegType type() const override { return RegType::NotWordPH; }
		std::string print() const override;
		bool execute(std::string_view substr, size_t& i)  override;
		~NotWordPH() = default;
	};

	class WhitespacePH : public PlaceHolder
	{
    public:
		WhitespacePH() = default;
		constexpr RegType type() const override { return RegType::WhitespacePH; }
		std::string print() const override;
		bool execute(std::string_view substr, size_t& i)  override;
		~WhitespacePH() = default;
	};

	class NotWhitespacePH : public PlaceHolder
	{
    public:
		NotWhitespacePH() = default;
		constexpr RegType type() const override { return RegType::NotWhitespacePH; }
		std::string print() const override;
		bool execute(std::string_view substr, size_t& i)  override;
		~NotWhitespacePH() = default;
	};

	class DotPH : public PlaceHolder
	{
    public:
		DotPH() = default;
		constexpr RegType type() const override { return RegType::DotPH; }
		std::string print() const override;
		bool execute(std::string_view substr, size_t& i)  override;
		~DotPH() = default;
	};


	class PerenRelated {}; // This is to make it easier to organize the peren markers and the peren containers in a list
	/* --- CONTAINER REGS ---*/

	class ParenContainer : public BasePart, public PerenRelated
	{
	public:
		ParenContainer() = default;
		
		ParenContainer(ParenContainer&& other) noexcept = default;

		void add_part(std::unique_ptr<BasePart> part);

		size_t size() const;

		std::unique_ptr<BasePart> pop_back_and_transfer();

		std::unique_ptr<BasePart>& get_back();

		std::string print() const override;

		void pop_back();

		void replace_back_part(std::unique_ptr<BasePart> part);

		constexpr RegType type() const override { return RegType::Parentheses; }
		
		bool execute(std::string_view substr, size_t& i) override;

		size_t charater_length() const override;
				
		~ParenContainer() = default;
	protected:
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

		std::string print() const override;

		bool execute(std::string_view substr, size_t& i)  override;

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

		bool execute(std::string_view substr, size_t& i)  override;

		std::string print() const override;

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

		std::string print() const override;

		bool execute(std::string_view substr, size_t& i)  override;

		~RepeatNumberedTimes() override = default;
	private:
		uint16_t _times;
	};

	class OrOp : public BasePart
	{
	public:
		OrOp() = default;

		OrOp(std::unique_ptr<BasePart> left) : _left(std::move(left)) { _right.reset(); }

		OrOp(BasePart* left) : _left(left) { _right.reset(); }

		size_t charater_length() const override;
		
		bool execute(std::string_view substr, size_t& i)  override;

		std::string print() const override;

		void set_right(std::unique_ptr<BasePart> part);

		constexpr RegType type() const override { return RegType::Or; }

		~OrOp() = default;
	private:
			std::unique_ptr<BasePart> _left;
			std::unique_ptr<BasePart> _right;
	};






	class StarOp : public RepeatPartsBase
	{
	public:

		StarOp() = default;
		constexpr RegType type() const override { return RegType::Star; }
		std::string print() const override;
		bool execute(std::string_view substr, size_t& i)  override;
		~StarOp() = default;
	};


	class PlusOp : public RepeatPartsBase // One or more times
	{
	public:
		PlusOp() = default;
		bool execute(std::string_view substr, size_t& i)  override;
		std::string print() const override;
		constexpr RegType type() const override { return RegType::Plus; }
		~PlusOp() override = default;

	};


	class OneOrZeroOp : public RepeatPartsBase
	{
    public:
		OneOrZeroOp() = default;
		constexpr RegType type() const override { return RegType::OneOrZero; }
		std::string print() const override;
		bool execute(std::string_view substr, size_t& i)  override;
		~OneOrZeroOp() override = default;
	};


	class StringPlacementPartsBase : public BasePart
	{
	public:
		StringPlacementPartsBase() = default;
		virtual ~StringPlacementPartsBase() = default;
	protected:
	};

	class StartOfString : public StringPlacementPartsBase
	{
	public:
		StartOfString() = default;
		constexpr RegType type() const override { return RegType::StartOfString; }
		std::string print() const override;
		bool execute(std::string_view substr, size_t& i)  override;
		size_t charater_length() const override;
		~StartOfString() override = default;
	private:
	};

	class EndOfString : public StringPlacementPartsBase
	{
	public:
		EndOfString() = default;
		constexpr RegType type() const override { return RegType::EndOfString; }
		std::string print() const override;
		bool execute(std::string_view substr, size_t& i)  override;
		size_t charater_length() const override;
		~EndOfString() override = default;
	private:
	};

	class WordBoundary : public StringPlacementPartsBase
	{
	public:
		WordBoundary() = default;
		bool execute(std::string_view substr, size_t& i)  override;
		size_t charater_length() const override;
		constexpr RegType type() const override { return RegType::WordBoundary; }
		std::string print() const override;
		~WordBoundary() override = default;

	};

	class NonWordBoundary : public StringPlacementPartsBase
	{
	public:
		NonWordBoundary() = default;
		bool execute(std::string_view substr, size_t& i)  override;
		size_t charater_length() const override;
		constexpr RegType type() const override { return RegType::NonWordBoundary; }
		std::string print() const override;
		~NonWordBoundary() = default;
	private:
	};


	class Backreference : public BasePart
	{
	public:
		Backreference(ParenContainer& referencedCont, uint16_t num);
		bool execute(std::string_view substr, size_t& i)  override;
		size_t charater_length() const override;
		constexpr RegType type() const override { return RegType::Backreference; }
		std::string print() const override;
		~Backreference() = default;
	public:
		uint16_t _num;
		ParenContainer& _referencedContainer;
	};


	class PerenMarker : public BasePart, public PerenRelated
	{
	public:
		PerenMarker(size_t prevI);
		bool execute(std::string_view substr, size_t& i)  override { return true; }
		const size_t get_prev_index() const;
		constexpr RegType type() const override { return RegType::PerenToAddHere; }
		size_t charater_length() const override { return 0; }
		~PerenMarker() override = default;

	private:
		const size_t _prevI;
	};
}
