#pragma once

class Memory
{
public:

	using ValueType = uint16_t;

	ValueType Read(ValueType adr);
};