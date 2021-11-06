
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

R VirtualMachine::RegisterName(ValueType code)
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

    auto sr1_v = (instr & mSR1) >> 6;
    auto sr1 = RegisterName(sr1_v);
        
    auto dr_v = (instr & mDR) >> 9;
    auto dr = RegisterName(dr_v);

    if (R::NREG == dr || R::NREG == sr1) {
        return false;
    }

    ValueType add_v{ 0 };

    if (instr & mMode)
    {
        add_v = SignExtend(instr & mImm5, 5);
    }
    else
    {
        auto sr2_v = instr & mSR2;
        auto sr2 = RegisterName(sr2_v);

        if (R::NREG == sr2) {
            return false;
        }

        add_v = m_(sr2);
    }

    if (sr1 == dr)
    {
        m_(dr) += add_v;
    }
    else
    {
        m_(dr) = m_(sr1) + add_v;
    }

    UpdateFlags(dr);
    
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
