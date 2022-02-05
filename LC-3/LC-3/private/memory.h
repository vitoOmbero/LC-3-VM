#pragma once
#include <fstream>

#ifdef WIN32
#undef max
#endif

class Memory
{
public:

	using ValueType = uint16_t;

	bool ReadObj(std::ifstream& obj_is);
	inline ValueType* Get(ValueType val) { return m_memory + val; };

	ValueType Read(ValueType addr);
	void Write(ValueType addr, ValueType val);

private:
	ValueType m_memory[std::numeric_limits<ValueType>::max()] = {};
};

#ifdef WIN32
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
