#include "memory.h"

#include <iostream>
#include <bit>

#ifdef WIN32
#include <Windows.h>
#include <conio.h>  // _kbhit


#endif // WIN32


template<typename T>
T swapBits(T x)
{
	return (x << (sizeof(T) * 8 / 2)) | (x >> (sizeof(T) * 8 / 2));
}

// NOTE: LC-3 memory mapped registers
const Memory::ValueType KBSR = 0xFE00; // keyboard status
const Memory::ValueType KBDR = 0xFE02; // keyboard data

bool Memory::ReadObj(std::ifstream& obj_is)
{
	ValueType origin;

	obj_is.read(reinterpret_cast<char*>(&origin), sizeof(origin));

	// NOTE: when VM's host is a little endian machine
	if constexpr (std::endian::native != std::endian::big)
	{
		origin = swapBits(origin);
	}

#ifdef WIN32
#undef max
#endif // WIN32
	// NOTE: LC-3 program size is limited
	auto maxRead = std::numeric_limits<ValueType>::max() - origin;
#ifdef WIN32
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

	ValueType* p = m_memory + origin;

	obj_is.read(reinterpret_cast<char*>(p), maxRead * sizeof(ValueType));
	auto read = obj_is.gcount();

	if constexpr (std::endian::native != std::endian::big)
	{
		while (read--)
		{
			*p = swapBits(*p);
			++p;
		}
	}

	return true;
}


Memory::ValueType CheckKey()
{
	HANDLE hStdin = INVALID_HANDLE_VALUE;
	return WaitForSingleObject(hStdin, 1000) == WAIT_OBJECT_0 && _kbhit();
}

Memory::ValueType Memory::Read(ValueType addr)
{
	if (addr == KBSR)
	{
		if (CheckKey())
		{
			m_memory[KBSR] = (1 << 15);
			//m_memory[KBDR] = getchar();
			std::cin.read(reinterpret_cast<char*>(&m_memory[KBDR]), sizeof(char));
		}
		else {
			m_memory[KBSR] = 0;
		}
	}
	return m_memory[addr];
}

void Memory::Write(ValueType addr, ValueType val)
{
	m_memory[addr] = val;
}
