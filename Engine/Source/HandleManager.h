#pragma once

#include <array>

template<size_t poolSize>
class HandleManager 
{
	struct Data
	{
		UINT index : 24;
		UINT generation : 8;
	};

	std::array<Data, poolSize> pool;

	UINT head = 0;
	UINT auto_generation = 0;

public:
	HandleManager()
	{
		UINT nextIndex = 0;
		for (Data& data : pool)
		{
			data.index = ++nextIndex;
			data.generation = 0;
		}
	}

	UINT allocHandle()
	{
		_ASSERT_EXPR(head < poolSize, "Handle pool exhausted");
		
		if (head < poolSize)
		{
			int slot = head; // Grab next free

			Data& data = pool[slot];
			head = data.index; // Update head

			data.index = slot;
			data.generation = ++auto_generation;

			// Guarantee 0 is always a invalid handle
			if (data.index == 0 && data.generation == 0)
				data.generation = ++auto_generation;

			return *reinterpret_cast<UINT*>(&data);
		}
		return 0;
	}

	void freeHandle(UINT handle)
	{
		_ASSERTE(validHandle(handle));

		Data& data = pool[indexFromHandle(handle)];

		int slot = data.index;

		data.index = head; // Link to current head
		head = slot; // Update head
	}

	UINT indexFromHandle(UINT handle) const
	{	
		_ASSERTE(validHandle(handle));
		Data data;
		*reinterpret_cast<UINT*>(&data) = handle;
		return data.index;
	}

	bool validHandle(UINT handle) const
	{
		if (handle == 0)
			return false;
		Data data;
		*reinterpret_cast<UINT*>(&data) = handle;
		if (data.index >= poolSize)
			return false;
		Data storedData = pool[data.index];
		return storedData.index == data.index && storedData.generation == data.generation; // Checks BOTH index AND generation
	}
};