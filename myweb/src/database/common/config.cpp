//bustub
#include"../a-include/common/config.h"

namespace bustub{
	std::atomic<bool> enableLogging(false);
	std::chrono::duration<int64_t> logTimeout = std::chrono::seconds(1);
	std::chrono::milliseconds cycleDetectionInterval = std::chrono::milliseconds(50);
}
