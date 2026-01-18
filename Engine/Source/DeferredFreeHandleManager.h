#pragma once

#include "HandleManager.h"

template<size_t poolSize>
class DeferredFreeHandleManager : public HandleManager<poolSize>
{
	struct DeferredData
	{
		UINT frame = 0;
		UINT handle = 0;
	};
	std::array<DeferredData, poolSize> deferredPool;
	UINT deferredHead = 0;

public:
	void deferRelease(UINT handle, UINT currentFrame)
	{
		//Solo para GPU handles
		_ASSERT_EXPR(HandleManager<poolSize>::validHandle(handle), L"Invalid handle");

		for (DeferredData& data : deferredPool)
		{
			if (data.handle == handle)
			{
				data.frame = currentFrame;
				return;
			}
		}

		deferredPool[deferredHead++] = { currentFrame, handle };
	}

	void collectGarbage(UINT lastCompletedFrame)
	{
		for (size_t i = 0; i < deferredHead;)
		{
			DeferredData& data = deferredPool[i];
			if (data.frame <= lastCompletedFrame)
			{
				HandleManager<poolSize>::freeHandle(data.handle);
				--deferredHead;
				deferredPool[i] = deferredPool[deferredHead];
			}
			else ++i;
		}
	}

	void forceCollectGarbage()
	{
		for (size_t i = 0; i < deferredHead; ++i)
		{
			DeferredData& data = deferredPool[i];
			HandleManager<poolSize>::freeHandle(data.handle);
		}

		deferredHead = 0;
	}
};