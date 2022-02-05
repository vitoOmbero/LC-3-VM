#pragma once

#include <iostream>
#include <vector>
#include <string_view>

#include "identifiers.h"
#include "private/memory.h"

#define OUT

using VMProgram = std::vector<int16_t>;

class VirtualMachine
{
public:
	using ValueType = int16_t;

	VirtualMachine(bool traceModeOn = true);

	void Run(const VMProgram& program);

private:
	/// <summary>
	/// Register values storage
	/// </summary>
	std::array<ValueType, static_cast<ValueType>(R::NREG)> m_register;
	bool m_traceMode;
	bool m_isRunning;
	const VMProgram* m_program;
	Memory m_mem;

private:
	void Show(OP opCode);

	void Skip(ValueType n);

	bool ProcessAddOperation(ValueType instr);
	bool ProcessAndOperation(ValueType instr);
	bool ProcessNotOperation(ValueType instr);
	bool ProcessBranchOperation(ValueType instr);
	bool ProcessJumpOperation(ValueType instr);
	bool ProcessJumpRegOperation(ValueType instr);
	bool ProcessLoadOperation(ValueType instr);
	bool ProcessLoadIndirectOperation(ValueType instr);
	bool ProcessLoadRegisterOperation(ValueType instr);
	bool ProcessLoadEffectiveAddressOperation(ValueType instr);
	bool ProcessStoreOperation(ValueType instr);
	bool ProcessStoreIndirectOperation(ValueType instr);
	bool ProcessStoreRegisterOperation(ValueType instr);
	bool ProcessTrapOperation(ValueType instr);
	ValueType SignExtend(ValueType x, int bit_count);

	/// <summary>
	/// Update Flags Register relatively the value in target register
	/// </summary>
	/// <param name="r">target register</param>
	void UpdateFlags(R r);

private:
	constexpr ValueType& m_(R regName) { return m_register[static_cast<ValueType>(regName)]; }
	ValueType Fetch() { return m_mem.Read(m_(R::PC)++); };
	inline R RegisterNameFromRegisterCode(ValueType code);

	R RegisterNameFromInstr(ValueType instruction);
	{

	}

};
