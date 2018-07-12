
#ifdef _MSC_VER
#endif

#include "FastXOR.h"
#include "Timer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

// 256k buffers we will test
#define MAX_BUFFER_SIZE (1024*256)

class TestFastXOR
{
public:
	TestFastXOR(void)
	{
		mBuffer = (uint8_t *)malloc(MAX_BUFFER_SIZE);
		mBufferA = (uint8_t *)malloc(MAX_BUFFER_SIZE);
		mBufferB = (uint8_t *)malloc(MAX_BUFFER_SIZE);
		srand(0);
	}

	~TestFastXOR(void)
	{
		free(mBuffer);
		free(mBufferA);
		free(mBufferB);
	}

	void speedTests(void)
	{
		printf("Running speed tests.\r\n");
		double slowTime = 0;
		double fastTime = 0;
		for (uint32_t i = 0; i < 10000; i++)
		{
			uint32_t randomOffset = rand() & 255;
			getRandomData(4, (MAX_BUFFER_SIZE - 256));

			memcpy(mBufferA + randomOffset, mBuffer, mDataLength);
			memcpy(mBufferB + randomOffset, mBuffer, mDataLength);

			{
				timer::Timer t;
				t.getElapsedSeconds();
				fastxor::slowXOR(mBufferA + randomOffset, mDataLength, mKey);
				slowTime += t.getElapsedSeconds();
			}
			{
				timer::Timer t;
				t.getElapsedSeconds();
				fastxor::fastXOR(mBufferB + randomOffset, mDataLength, mKey);
				fastTime += t.getElapsedSeconds();
			}
		}
		printf("SlowXOR: %0.4f seconds.\r\n", slowTime);
		printf("FastXOR: %0.4f seconds.\r\n", fastTime);
		double ratio = slowTime / fastTime;
		printf("FastXOR is %0.2f times faster than slow XOR.\r\n", ratio);
	}

	void validate(void)
	{
		printf("Running validation tests.\r\n");

		for (uint32_t i = 0; i < 10000; i++)
		{
			uint32_t randomOffset = rand() & 255;
			getRandomData(4, (MAX_BUFFER_SIZE-256));

			memcpy(mBufferA+randomOffset, mBuffer, mDataLength);
			memcpy(mBufferB+randomOffset, mBuffer, mDataLength);

			fastxor::slowXOR(mBufferA+randomOffset, mDataLength, mKey);
			fastxor::fastXOR(mBufferB+randomOffset, mDataLength, mKey);

			if (memcmp(mBufferA + randomOffset, mBufferB + randomOffset, mDataLength) == 0)
			{
				if ((i % 500) == 0)
				{
					printf("%d tests passed.\r\n", i);
				}
//				printf("Test: %d of length: %d with offset:%d passed!\r\n", i, mDataLength, randomOffset);
			}
			else
			{
				printf("**** TEST FAILED *****!!!!\r\n");
				assert(0);
			}
		}
	}

	// Get a random XOR mask key (using high resolution timer)
	void getKey(void)
	{
		uint32_t key = rand();
		memcpy(mKey, &key, sizeof(mKey));
	}

	void getRandomData(uint32_t minSize,uint32_t maxSize)
	{
		// seed the random number generator
		getKey();
		if (maxSize > MAX_BUFFER_SIZE)
		{
			maxSize = MAX_BUFFER_SIZE;
		}
		uint32_t diff = maxSize - minSize;
		mDataLength = (rand() % diff) + minSize;
		for (uint32_t i = 0; i < mDataLength; i++)
		{
			mBuffer[i] = (rand() % (127 - 32)) + 32; // ASCII characters
		}
	}

	uint8_t		mKey[4];
	uint32_t	mDataLength{ 0 };
	// non-xor test data
	uint8_t		*mBuffer{ nullptr };
	// xor using fast method
	uint8_t		*mBufferA{ nullptr };
	// xor using slow method
	uint8_t		*mBufferB{ nullptr };

};

int main(void)
{

	TestFastXOR tfx;
	tfx.validate();
	tfx.speedTests();

	return 0;
}



