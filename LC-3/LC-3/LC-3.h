#pragma once

#include <iostream>
#include <vector>
#include <string_view>
#include <filesystem>

#include "identifiers.h"
#include "private/memory.h"

using VMProgram = std::vector<uint16_t>;

class VirtualMachine
{
public:
	using ValueType = uint16_t;

	VirtualMachine(bool traceModeOn = true);

	bool LoadObj(std::filesystem::path obj);
	void Run();

private:
	/// <summary>
	/// Register values storage
	/// </summary>
	std::array<ValueType, static_cast<ValueType>(R::NREG)> m_register;
	bool m_traceMode;
	bool m_isRunning;
	Memory m_mem;

private:
	void Show(OP opCode);

	void Skip(ValueType n);

	bool ProcessAddAndOperations(ValueType instr, bool ADD = true);
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
	inline TR TrapNameFromTrapCode(ValueType code);

};
