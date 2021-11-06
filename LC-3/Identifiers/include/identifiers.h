#pragma once
#pragma  warning (disable : 26812)
#include <string_view>
#include <array>

/// <summary>
/// Register names
/// </summary>
enum class R
{
	R0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,
	PC,
	COND,

	NREG
};

/// <summary>
/// OpCodes
/// </summary>
enum class OP
{
	FIRST,

	BR = FIRST,     /* branch */
	ADD,			/* add  */
	LD,				/* load */
	ST,				/* store */
	JSR,			/* jump register */
	AND,			/* bitwise and */
	LDR,			/* load register */
	STR,			/* store register */
	RTI,			/* unused */
	NOT,			/* bitwise not */
	LDI,			/* load indirect */
	STI,			/* store indirect */
	JMP,			/* jump */
	RES,			/* reserved (unused) */
	LEA,			/* load effective address */
	TRAP,			/* execute trap */

	NOP,
};

inline static const int kInstructionSizeInBits = 4;

/// <summary>
/// Conditional flags
/// </summary>
enum class FL
{
	POS = 1,
	ZRO = 1 << 1,
	NEG = 1 << 2
};

static inline std::array<std::string_view, static_cast<size_t>(OP::NOP)> g_instruction =
{
	"BR",
	"ADD",	
	"LD",	
	"ST",	
	"JSR",	
	"AND",	
	"LDR",	
	"STR",	
	"RTI",	
	"NOT",	
	"LDI",	
	"STI",	
	"JMP",	
	"RES",	
	"LEA",	
	"TRAP"	
};

static inline std::array<std::string_view, static_cast<size_t>(R::NREG)> g_register =
{
	"R0",
	"R1",
	"R2",
	"R3",
	"R4",
	"R5",
	"R6",
	"R7",
	"PC",
	"COND"
};

inline const std::string_view Str(OP opcode)
{
	return g_instruction[static_cast<size_t>(opcode)];
};

inline const std::string_view Str(R regCode)
{
	return g_register[static_cast<size_t>(regCode)];
};

inline static const int kNotIndex{ -1 };
template<int N> 
int64_t FindIndex(const std::array<std::string_view, N>& arr, const std::string_view& str)
{
	int index{ kNotIndex };
	auto it = std::find(arr.begin(), arr.end(), str);
	if (it != arr.end())
	{
		index = std::distance(arr.begin(), it);
	}
	return index;
};

/// <summary>
/// Detect if instruction is known
/// </summary>
/// <param name="str">token</param>
/// <returns>index in <see cref="g_instruction"/></returns>
inline int64_t FindInstruction(const std::string_view& str)
{
	return FindIndex(g_instruction, str);
}

inline int64_t FindRegister(const std::string_view& str)
{
	return FindIndex(g_register, str);
}

