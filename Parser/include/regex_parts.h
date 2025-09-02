#pragma once


#include "config.h"


namespace parser
{

	enum class RegType
	{
		Base,

		Parentheses, // To group parts
		Bracket, // To define a set of characters
		CurlyBracket, // To define a limit of repetitions

		NotNumberPH,
		NumberPH,
		WhitespacePH,
		NotWhitespacePH,
		DotPH,

		Character,
		Or,
		Dot,
		Word,
		Whitespace,
		Star,
		Number,
		Dash,
		Plus,
		RepeatNumberedTimes,
		RepeatRangedTimes,
		OneOrZero,
		StartOfString,
		EndOfString,
		WordBoundary,
		NonWordBoundary,
		Backreference
	};

	class BasePart
	{
	public:

		virtual constexpr RegType type() const = 0;

		virtual ~BasePart() = default;
	};

	class PlaceHolder : public BasePart
	{};

	class NotNumberPH : public PlaceHolder
	{
		NotNumberPH() = default;
		constexpr RegType type() const override { return RegType::NotNumberPH; }
		~NotNumberPH() override = default;
	};

	class NumberPH : public PlaceHolder
	{
		public:
		NumberPH() = default;
		constexpr RegType type() const override { return RegType::NumberPH; }
		~NumberPH() override = default;
	};

	class WordPH : public PlaceHolder
	{
		public:
		WordPH() = default;
		constexpr RegType type() const override { return RegType::Word; }
		~WordPH() override = default;
	};

	class NotWordPH : public PlaceHolder
	{
		public:
		NotWordPH() = default;
		constexpr RegType type() const override { return RegType::NonWordBoundary; }
		~NotWordPH() override = default;
	};

	class WhitespacePH : public PlaceHolder
	{
		public:
		WhitespacePH() = default;
		constexpr RegType type() const override { return RegType::WhitespacePH; }
		~WhitespacePH() override = default;
	};
	class NotWhitespacePH : public PlaceHolder
	{
		public:
		NotWhitespacePH() = default;
		constexpr RegType type() const override { return RegType::NotWhitespacePH; }
		~NotWhitespacePH() override = default;
	};

	class DotPH : public PlaceHolder
	{
		public:
		DotPH() = default;
		constexpr RegType type() const override { return RegType::DotPH; }
		~DotPH() override = default;
	};


	/* --- CONTAINER REGS ---*/

	class Container : public BasePart
	{
	public:
		Container() = default;

		void add_part(std::unique_ptr<BasePart>& part) { _parts.push_back(std::move(part)); }
		 
		BasePart* part_back_ptr()
		{
			return _parts.back().get();
		}

		~Container() override = default;
	protected:
		std::vector<std::unique_ptr<BasePart>> _parts;
		std::unique_ptr<BasePart> _quantifier;
	};


	class ParenContainer : public Container
	{
	public:
		ParenContainer() = default;

		constexpr RegType type() const override { return RegType::Parentheses; }

		void set_quantifier(std::unique_ptr<BasePart>& quantifier) { _quantifier = std::move(quantifier); }

		~ParenContainer() override = default;
	private:
		std::vector<std::unique_ptr<BasePart>> _parts;
		std::unique_ptr<BasePart> _quantifier;
	};


	class BracketContainer : public Container
	{
	public:
		BracketContainer() = default;

		constexpr RegType type() const override { return RegType::Bracket; }

		void set_quantifier(std::unique_ptr<BasePart>& quantifier) { _quantifier = std::move(quantifier); }

		void set_not() { _not = true; }

		~BracketContainer() override = default;
	private:
		bool _not = false;
		std::vector<std::shared_ptr<CharacterPart>> _parts;
		std::unique_ptr<BasePart> _quantifier;
	};


	class CurlyBracketContainer : public Container
	{
	public:
		CurlyBracketContainer() = default;

		constexpr RegType type() const override { return RegType::CurlyBracket; }

		void set_quantifier(std::unique_ptr<BasePart>& quantifier) { _quantifier = std::move(quantifier); }

		~CurlyBracketContainer() override = default;

	};


	class RealValue : public BasePart
	{};

	class CharacterPart : public RealValue
	{
	public:
		CharacterPart(char c) : _char(c) {}

		constexpr RegType type() const override { return RegType::Character; }

		~CharacterPart() override = default;
	private:
		char _char;
	};

	class NumberPart : public RealValue
	{
	public:
		NumberPart(std::string_view number) 
		{ 
			for (char const& chr : number)
			{
				if (isdigit(chr))
				{
					_number = _number * 10 + (chr - '0');
				}
				else
				{
					_number = 0;
					break;
				}
			}
		}

		constexpr RegType type() const override { return RegType::Number; }
		
		int get_number() const { return _number; }
		
		~NumberPart() override = default;
	private:
		int _number = 0;
	};


	class Operator : public BasePart
	{};

	class OrOp : public Operator
	{
	public:
		OrOp(std::array<std::unique_ptr<BasePart>, 2>& items) : options(std::move(items)) {}

		OrOp(std::unique_ptr<BasePart>& part) : options{ std::move(part), nullptr } {}

		void add_branch(std::unique_ptr<BasePart>& part) 
		{ 
			for (auto& item : options)
			{
				if (item == nullptr)
				{
					item = std::move(part);
					return;
				}
			}
		}

		constexpr RegType type() const override { return RegType::Or; }

		~OrOp() override = default;
	private:
		std::array<std::unique_ptr<BasePart>, 2> options;
	};

	class StarOp : public Operator
	{
	public:
		StarOp(std::unique_ptr<BasePart>& part) { _repetition = std::move(part); };

		constexpr RegType type() const override { return RegType::Star; }

		~StarOp() override = default;
	private:
		std::unique_ptr<BasePart> _repetition;
	};


	class PlusOp : public Operator
	{
	public:
		PlusOp(std::shared_ptr<BasePart>& part) { _repetition = part; };

		constexpr RegType type() const override { return RegType::Plus; }

		~PlusOp() override = default;
	private:
		std::shared_ptr<BasePart> _repetition;
	};


	class OneOrZeroOp : public Operator
	{
		public:
		OneOrZeroOp(std::shared_ptr<BasePart>& part) { _repetition = part; };
		constexpr RegType type() const override { return RegType::OneOrZero; }
		~OneOrZeroOp() override = default;
	private:
		std::shared_ptr<BasePart> _repetition;
	};


	class DashOp : public Operator
	{
		public:
		DashOp(std::unique_ptr<BasePart>& part) { _left = std::move(part); };

		void set_right(std::unique_ptr<BasePart>& part) { _right = std::move(part); }

		constexpr RegType type() const override { return RegType::Dash; }
		


		~DashOp() override = default;
	private:
		std::unique_ptr<BasePart> _left;
		std::unique_ptr<BasePart> _right;
	};


	class WhitespacePart : public BasePart
	{
	public:
		WhitespacePart() = default;
		
		constexpr RegType type() const override { return RegType::Whitespace; }

		~WhitespacePart() override = default;
	private:
	};

	class RepeatPartsBase : public BasePart
	{
	public:
		RepeatPartsBase() = default;

		virtual ~RepeatPartsBase() = default;
	protected:
		std::shared_ptr<BasePart> _repetition; 
	};



	class RepeatNumberedTimesPart : public RepeatPartsBase
	{
	public:
		RepeatNumberedTimesPart(std::shared_ptr<BasePart>& part, int times) : _times(times) { _repetition = part; };

		constexpr RegType type() const override { return RegType::RepeatNumberedTimes; }

		~RepeatNumberedTimesPart() override = default;
	private:
		int _times;
	};


	enum class MinOrMaxVal
	{
		Min = 0,
		Max,
	};

	class RepeatRangedTimesPart : public RepeatPartsBase
	{
	public:
		RepeatRangedTimesPart(std::shared_ptr<BasePart>& part, int min, int max) : _min(min), _max(max) { _repetition = part; };
		
		RepeatRangedTimesPart(std::shared_ptr<BasePart>& part, int x, MinOrMaxVal minOrMax) 
		{
			_repetition = part;
			minOrMax == MinOrMaxVal::Max ? _max = x : _min = x;
		};

		constexpr RegType type() const override { return RegType::RepeatRangedTimes; }

		~RepeatRangedTimesPart() override = default;
	private:
		std::optional<int> _min;
		std::optional<int> _max;
	};

	class OneOrZeroPart : public RepeatPartsBase
	{
	public:
		OneOrZeroPart(std::shared_ptr<BasePart>& part) { _repetition = part; };

		constexpr RegType type() const override { return RegType::OneOrZero; }

		~OneOrZeroPart() override = default;
	private:
	};

	//class BackreferencePart : public BasePart
	//{
	//public:
	//	BackreferencePart(int groupNumber) : _groupNumber(groupNumber) {}

	//	constexpr RegType type() const override { return RegType::Backreference; }

	//	~BackreferencePart() override = default;
	//private:
	//	int _groupNumber;
	//};

	class StringPlacementPartsBase : public BasePart
	{
	public:
		StringPlacementPartsBase() = default;
		virtual ~StringPlacementPartsBase() = default;
	protected:
	};

	class StartOfStringPart : public StringPlacementPartsBase
	{
	public:
		StartOfStringPart() = default;

		constexpr RegType type() const override { return RegType::StartOfString; }

		~StartOfStringPart() override = default;
	private:
	};

	class EndOfStringPart : public StringPlacementPartsBase
	{
	public:
		EndOfStringPart() = default;

		constexpr RegType type() const override { return RegType::EndOfString; }

		~EndOfStringPart() override = default;
	private:
	};

	class WordBoundaryPart : public StringPlacementPartsBase
	{
	public:
		WordBoundaryPart() = default;

		constexpr RegType type() const override { return RegType::WordBoundary; }

		~WordBoundaryPart() override = default;
	private:

	};

	class NonWordBoundaryPart : public StringPlacementPartsBase
	{
	public:
		NonWordBoundaryPart() = default;

		constexpr RegType type() const override { return RegType::NonWordBoundary; }

		~NonWordBoundaryPart() override = default;
	private:
	};

	using CharOrNumber = std::variant<CharacterPart, NumberPart>;

	class DashPart : public BasePart
	{
	public:
		DashPart(CharacterPart left, CharacterPart right)
			: _partType(PartType::Character), _left(std::move(left)), _right(std::move(right)) {
		}

		DashPart(NumberPart left, NumberPart right)
			: _partType(PartType::Number), _left(std::move(left)), _right(std::move(right)) {
		}

	private:
		enum class PartType 
		{ 
			Character, 
			Number 
		} _partType;

		CharOrNumber _left;
		CharOrNumber _right;
	};

}