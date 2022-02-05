
#include "LC-3.h"

#ifdef WIN32
#undef OUT
#undef IN
#undef max
#endif

VirtualMachine::VirtualMachine(bool traceModeOn) :
	m_traceMode(traceModeOn),
	m_isRunning(false),
	m_register{}
{
	// NOTE: program for LC-3 starts here
	m_(R::PC) = 0x3000;
}

bool VirtualMachine::LoadObj(std::filesystem::path obj)
{
	std::ifstream is(obj, std::ios::in | std::ios::binary);
	if (!is) {
		std::cerr << "Some problems have acquired when opening the file. Does file exist? ";
		std::cerr << obj;
		return false;
	};

	if (!is.is_open()) {
		std::cerr << "Bad filename for .obj file " << obj << '\n';
		return false;
	}

	return m_mem.ReadObj(is);
}

void VirtualMachine::Run()
{

	if (m_traceMode)
	{
		std::cout << "LC-3 VM: is running..." << std::endl;
	}

	m_isRunning = true;

	while (m_isRunning)
	{
		auto instruction = Fetch();
		//ValueType opcode = instruction >> ((sizeof(ValueType) - kInstructionSizeInBits));
		ValueType opcode = instruction >> 12;

		if ((opcode > static_cast<ValueType>(OP::NOP)) ||
			(opcode < static_cast<ValueType>(OP::FIRST))) {
			std::cerr << "Unknown opcode!\n";
			return;
		}
		auto ic = static_cast<OP>(opcode);

		if (m_traceMode)
		{
			Show(ic);
		}


		switch (ic)
		{
		case OP::ADD:
			m_isRunning = ProcessAddAndOperations(instruction);
			break;
		case OP::AND:
			m_isRunning = ProcessAddAndOperations(instruction, false);
			break;
		case OP::NOT:
			m_isRunning = ProcessNotOperation(instruction);
			break;
		case OP::BR:
			m_isRunning = ProcessBranchOperation(instruction);
			break;
		case OP::JMP:
			m_isRunning = ProcessJumpOperation(instruction);
			break;
		case OP::JSR:
			m_isRunning = ProcessJumpRegOperation(instruction);
			break;
		case OP::LD:
			m_isRunning = ProcessLoadOperation(instruction);
			break;
		case OP::LDI:
			m_isRunning = ProcessLoadIndirectOperation(instruction);
			break;
		case OP::LDR:
			m_isRunning = ProcessLoadRegisterOperation(instruction);
			break;
		case OP::LEA:
			m_isRunning = ProcessLoadEffectiveAddressOperation(instruction);
			break;
		case OP::ST:
			m_isRunning = ProcessStoreOperation(instruction);
			break;
		case OP::STI:
			m_isRunning = ProcessStoreIndirectOperation(instruction);
			break;
		case OP::STR:
			m_isRunning = ProcessStoreRegisterOperation(instruction);
			break;
		case OP::TRAP:
			m_isRunning = ProcessTrapOperation(instruction);
			break;
		case OP::RES:
		case OP::RTI:
		default:
			m_isRunning = false;
			break;
		}

	}
}

void VirtualMachine::Show(OP opCode)
{
	if (m_traceMode) {
		std::cout << "OpCode: " << Str(opCode) << std::endl;
	}
}

R VirtualMachine::RegisterNameFromRegisterCode(ValueType code)
{
	if (code > static_cast<ValueType>(R::LAST) ||
		(code < static_cast<ValueType>(R::FIRST)))
	{
		return R::NREG;
	}
	return static_cast<R>(code);
}

inline TR VirtualMachine::TrapNameFromTrapCode(ValueType code)
{
	if (code > static_cast<ValueType>(TR::LAST) ||
		(code < static_cast<ValueType>(TR::FIRST)))
	{
		return TR::NTR;
	}
	return static_cast<TR>(code);
}

bool VirtualMachine::ProcessAddAndOperations(ValueType instr, bool addOpFlag)
{
	// 2 modes

	//bits |15   12|11 9|8   6| 5 | 4 3 | 2  0 |
	//data | 0001  | DR | SR1 | 0 | 00  | SR2  |  register mode
	//data | 0001  | DR | SR1 | 1 |   imm5     |  immediate mode
	const ValueType mOpCode = 0b1111 << 12;
	const ValueType mDR = 0b111 << 9;
	const ValueType mSR1 = 0b111 << 6;
	const ValueType mMode = 1 << 5;
	const ValueType mSR2 = 0b111;
	const ValueType mImm5 = 0b11111;

	auto srcReg1_val = (instr & mSR1) >> 6;
	auto srcReg1 = RegisterNameFromRegisterCode(srcReg1_val);
	auto destReg_val = (instr & mDR) >> 9;
	auto destReg = RegisterNameFromRegisterCode(destReg_val);
	if (R::NREG == destReg || R::NREG == srcReg1) {
		return false;
	}

	if (instr & mMode)
	{
		addOpFlag ?
			m_(destReg) = m_(srcReg1) + SignExtend(instr & mImm5, 5) :
			m_(destReg) = m_(srcReg1) & SignExtend(instr & mImm5, 5);
	}
	else
	{
		auto srcReg2_val = instr & mSR2;
		auto srcReg2 = RegisterNameFromRegisterCode(srcReg2_val);
		if (R::NREG == srcReg2) {
			return false;
		}

		addOpFlag ?
			m_(destReg) = m_(srcReg1) + m_(srcReg2) :
			m_(destReg) = m_(srcReg1) & m_(srcReg2);
	}

	UpdateFlags(destReg);
	return true;
}

bool VirtualMachine::ProcessNotOperation(ValueType instr)
{
	//bits |15   12|11 9|8   6| 5 |4         0|
	//data | 1001  | DR | SR  | 1 |   11111   |
	const ValueType mDR = 0b111 << 9;
	const ValueType mSR = 0b111 << 6;

	auto srcReg1_val = (instr & mSR) >> 6;
	auto srcReg1 = RegisterNameFromRegisterCode(srcReg1_val);
	auto destReg_val = (instr & mDR) >> 9;
	auto destReg = RegisterNameFromRegisterCode(destReg_val);
	if (R::NREG == destReg || R::NREG == srcReg1) {
		return false;
	}

	m_(destReg) = ~m_(srcReg1);

	UpdateFlags(destReg);
	return true;
}

bool VirtualMachine::ProcessBranchOperation(ValueType instr)
{
	// bits |15    12| 11 | 10 | 9 |8         0|
	// data |  0000  | n  | z  | p | PCoffset9 |
	const ValueType mPCoffset9 = 0x1FF;
	//const ValueType NZP = instr >> 9;

	if ((instr >> 9) & m_(R::COND)) {
		m_(R::PC) += SignExtend(instr & mPCoffset9, 9);
	}

	return true;
}

bool VirtualMachine::ProcessJumpOperation(ValueType instr)
{
	//bits |15   12|11  9|8     6| 5            0 |
	//data | 1100  | 000 | BaseR |    000000      | JMP
	//data | 1100  | 000 |  111  |    000000      | RET
	const ValueType mBaseR = 0b111 << 6;

	auto reg_val = (instr & mBaseR) >> 9;
	auto reg = RegisterNameFromRegisterCode(reg_val);
	if (R::NREG == reg) {
		return false;
	}

	m_(R::PC) = m_(reg);

	return true;
}

bool VirtualMachine::ProcessJumpRegOperation(ValueType instr)
{
	//bits |15   12| 11  | 10                  0 |
	//data | 0100  | 1   |      PCoffset11       | JSR

	//bits |15   12|11  9|8     6| 5            0 |
	//data | 0100  | 000 | BaseR |    000000      | JSSR

	const ValueType mBit11 = 0b1 << 11;
	const ValueType mBaseR = 0b111 << 6;
	const ValueType mPCoffset11 = 0x7FF;

	auto reg_val = (instr & mBaseR) >> 9;
	auto reg = RegisterNameFromRegisterCode(reg_val);
	if (R::NREG == reg) {
		return false;
	}

	m_(R::R7) = m_(R::PC);
	if (instr & mBit11) {
		m_(R::PC) += SignExtend(instr & mPCoffset11, 11);
	}
	else {
		m_(R::PC) = m_(reg);
	}

	return true;
}

bool VirtualMachine::ProcessLoadOperation(ValueType instr)
{
	//bits |15   12|11  9|8              0 |
	//data | 0010  |  DR |  PCoffset9      |

	const ValueType mDR = 0b111 << 9;
	const ValueType mPCoffset9 = 0x1FF;

	auto destReg_val = (instr & mDR) >> 9;
	auto destReg = RegisterNameFromRegisterCode(destReg_val);
	if (R::NREG == destReg) {
		return false;
	}

	m_(destReg) = m_mem.Read(m_(R::PC) + SignExtend(instr & mPCoffset9, 9));

	UpdateFlags(destReg);
	return true;
}

VirtualMachine::ValueType VirtualMachine::SignExtend(VirtualMachine::ValueType x, int bit_count)
{
	if ((x >> (bit_count - 1)) & 1) {
		x |= (0xFFFF << bit_count);
	}
	return x;
}

void VirtualMachine::UpdateFlags(R r)
{
	if (m_(r) == 0)
	{
		m_(R::COND) = static_cast<ValueType>(FL::ZRO);
	}
	else if (m_(r) >> 15)
	{
		m_(R::COND) = static_cast<ValueType>(FL::NEG);
	}
	else
	{
		m_(R::COND) = static_cast<ValueType>(FL::POS);
	}
}

bool VirtualMachine::ProcessLoadIndirectOperation(ValueType instr)
{
	// bits |15    12|11  9|8         0|
	// data | OpCode |  DR | PCoffset9 |
	const ValueType mOpCode = 0b1111 << 12;
	const ValueType mDR = 0b111 << 9;
	const ValueType mPCoffset9 = 0x1FF;

	auto destReg_val = (instr & mDR) >> 9;
	auto destReg = RegisterNameFromRegisterCode(destReg_val);
	if (R::NREG == destReg) {
		return false;
	}

	m_(destReg) = m_mem.Read(m_mem.Read(m_(R::PC) + SignExtend(instr & mPCoffset9, 9)));

	UpdateFlags(destReg);
	return true;
}

bool VirtualMachine::ProcessLoadRegisterOperation(ValueType instr)
{
	//bits |15   12|11  9|8     6| 5            0 |
	//data | 0110  |  DR | BaseR |    PCoffset6   |
	const ValueType mDR = 0b111 << 9;
	const ValueType mPCoffset6 = 0x3F;
	const ValueType mBaseR = 0b111 << 6;

	auto destReg_val = (instr & mDR) >> 9;
	auto destReg = RegisterNameFromRegisterCode(destReg_val);
	if (R::NREG == destReg) {
		return false;
	}

	auto reg_val = (instr & mBaseR) >> 9;
	auto reg = RegisterNameFromRegisterCode(reg_val);
	if (R::NREG == reg) {
		return false;
	}

	m_(destReg) = m_mem.Read(m_(reg) + SignExtend(instr & mPCoffset6, 6));

	UpdateFlags(destReg);
	return true;
}

bool VirtualMachine::ProcessLoadEffectiveAddressOperation(ValueType instr)
{
	//bits |15   12|11  9|8              0 |
	//data | 1110  |  DR |  PCoffset9      |
	const ValueType mDR = 0b111 << 9;
	const ValueType mPCoffset9 = 0x1FF;

	auto destReg_val = (instr & mDR) >> 9;
	auto destReg = RegisterNameFromRegisterCode(destReg_val);
	if (R::NREG == destReg) {
		return false;
	}

	m_(destReg) = m_(R::PC) + SignExtend(instr & mPCoffset9, 9);

	UpdateFlags(destReg);
	return true;
}

bool VirtualMachine::ProcessStoreOperation(ValueType instr)
{
	//bits |15   12|11  9|8              0 |
	//data | 0011  |  SR |  PCoffset9      |
	const ValueType mSR = 0b111 << 9;
	const ValueType mPCoffset9 = 0x1FF;

	auto srcReg_val = (instr & mSR) >> 9;
	auto srcReg = RegisterNameFromRegisterCode(srcReg_val);
	if (R::NREG == srcReg) {
		return false;
	}

	m_mem.Write(m_(R::PC) + SignExtend(instr & mPCoffset9, 9), m_(srcReg));

	return true;
}

bool VirtualMachine::ProcessStoreIndirectOperation(ValueType instr)
{
	//bits |15   12|11  9|8              0 |
	//data | 1011  |  SR |  PCoffset9      |

	const ValueType mSR = 0b111 << 9;
	const ValueType mPCoffset9 = 0x1FF;

	auto srcReg_val = (instr & mSR) >> 9;
	auto srcReg = RegisterNameFromRegisterCode(srcReg_val);
	if (R::NREG == srcReg) {
		return false;
	}

	m_mem.Write(m_mem.Read(m_(R::PC) + SignExtend(instr & mPCoffset9, 9)), m_(srcReg));

	return true;
}

bool VirtualMachine::ProcessStoreRegisterOperation(ValueType instr)
{
	//bits |15   12|11  9|8     6| 5            0 |
	//data | 0111  |  SR | BaseR |    PCoffset6   |
	const ValueType mSR = 0b111 << 9;
	const ValueType mPCoffset6 = 0x3F;
	const ValueType mBaseR = 0b111 << 6;

	auto srcReg_val = (instr & mSR) >> 9;
	auto srcReg = RegisterNameFromRegisterCode(srcReg_val);
	if (R::NREG == srcReg) {
		return false;
	}

	auto reg_val = (instr & mBaseR) >> 9;
	auto reg = RegisterNameFromRegisterCode(reg_val);
	if (R::NREG == reg) {
		return false;
	}

	m_mem.Write(m_(reg) + SignExtend(instr & mPCoffset6, 6), m_(srcReg));

	UpdateFlags(srcReg);
	return true;
}

bool VirtualMachine::ProcessTrapOperation(ValueType instr)
{
	//bits |15   12|11  8|8              0 |
	//data | 1111  |0000 |  trapvect8      |
	const ValueType mtrapvect8 = 0x1FF;

	auto tr_val = (instr & mtrapvect8);
	auto tr = TrapNameFromTrapCode(tr_val);
	if (TR::NTR == tr) {
		return false;
	}

	switch (tr)
	{
	case TR::IN:
	{
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Enter a character: ";
		char c;
		std::cin.getline(&c, sizeof(c));
		std::cout << c;
		std::cout.flush();
		m_(R::R0) = c;
		UpdateFlags(R::R0);
	}
	break;
	case TR::GETC:
	{
		char c;
		std::cin.getline(&c, sizeof(c));
		m_(R::R0) = c;
		UpdateFlags(R::R0);
	}
	break;
	case TR::OUT:
		std::cout << static_cast<char>(m_(R::R0));
		std::cout.flush();
		break;
	case TR::PUTS:
	{
		for (ValueType* _char = m_mem.Get(m_(R::R0)); *_char; ++_char)
		{
			std::cout.put(static_cast<char>(*_char));
		}
		std::cout.flush();
	}
	break;
	case TR::PUTSP:
	{
		for (ValueType* _char = m_mem.Get(m_(R::R0)); *_char; ++_char)
		{
			char ch1 = (*_char) & 0xFF;
			std::cout.put(ch1);
			char ch2 = (*_char) >> 8;
			if (ch2) {
				std::cout.put(ch2);
			}
		}
		std::cout.flush();
	}
	break;
	case TR::HALT:
	default:
		std::cout << "HALT\n";
		return false;
	}

	return true;
}
