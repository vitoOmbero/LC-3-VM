
#include "LC-3.h"

VirtualMachine::VirtualMachine(bool traceModeOn) :
	m_traceMode(traceModeOn),
	m_isRunning(false),
	m_program(nullptr)
{
	// TODO: const for Memory storage size
	m_(R::PC) = 0x3000;
}

void VirtualMachine::Run(const VMProgram& program)
{

	if (m_traceMode)
	{
		std::cout << "LC-3 VM: is running..." << std::endl;
	}

	m_isRunning = true;
	m_program = &program;

	while (m_isRunning)
	{
		auto instruction = Fetch();
		auto opcode = instruction >> (sizeof(ValueType) - kInstructionSizeInBits);

        if ((opcode > static_cast<ValueType>(OP::NOP)) ||
            (opcode < static_cast<ValueType>(OP::FIRST))) {
            Skip(1);
            continue;
        }
        auto ic = static_cast<OP>(opcode);

        if (m_traceMode)
        {
            Show(ic);
        }

            
        switch (ic)
        {
        case OP::ADD:
            m_isRunning = ProcessAddOperation(instruction);
        break;
        case OP::AND:
            ProcessAndOperation(instruction);
        break;
        case OP::NOT:
            ProcessNotOperation(instruction);
        break;
        case OP::BR:
            ProcessBranchOperation(instruction);
        break;
        case OP::JMP:
            ProcessJumpOperation(instruction);
        break;
        case OP::JSR:
            ProcessJumpRegOperation(instruction);
        break;
        case OP::LD:
            ProcessLoadOperation(instruction);
        break;
        case OP::LDI:
            ProcessLoadIndirectOperation(instruction);
        break;
        case OP::LDR:
            ProcessLoadRegisterOperation(instruction);
        break;
        case OP::LEA:
            ProcessLoadEffectiveAddressOperation(instruction);
        break;
        case OP::ST:
            ProcessStoreOperation(instruction);
        break;
        case OP::STI:
            ProcessStoreIndirectOperation(instruction);
        break;
        case OP::STR:
            ProcessStoreRegisterOperation(instruction);
        break;
        case OP::TRAP:
            ProcessTrapOperation(instruction);
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
    if (code > static_cast<ValueType>(R::NREG) ||
        (code < static_cast<ValueType>(R::R0)))
    {
        return R::NREG;
    }
    return static_cast<R>(code);
}

bool VirtualMachine::ProcessAddOperation(ValueType instr)
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
        m_(destReg) = m_(srcReg1) + SignExtend(instr & mImm5, 5);
    }
    else
    {
        auto srcReg2_val = instr & mSR2;
        auto srcReg2 = RegisterNameFromRegisterCode(srcReg2_val);

        if (R::NREG == srcReg2) {
            return false;
        }

        m_(destReg) = m_(srcReg1) + m_(srcReg2);
    }

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
    const ValueType mPCoffset9 = 0b111111111;



    return true;
}
