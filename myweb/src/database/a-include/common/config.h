//BusTub

#pragma once
#include <atomic>
#include <chrono>
#include <cstdint>

namespace bustub{
	extern std::chrono::milliseconds cycleDetetionInterval;
	extern std::atomic<bool> enableLogging;
	extern std::chrono::duration<int64_t> logTimeout;
	
	static constexpr int INVALID_PAGE_ID = -1;
	static constexpr int INVALID_TXN_ID = -1;
	static constexpr int HEADER_PAGE_ID = 0;
	static constexpr int PAGE_SIZE = 4096;
	static constexpr int BUFFER_POOL_SIZE = 10;
	static constexpr int LOG_BUFFER_SIZE = ((BUFFER_POOL_SIZE + 1) * PAGE_SIZE);
	
	using frame_id_t = int32_t;
	using page_id_t = int32_t;
	using txn_id_t = int32_t;
	using slot_offset_t = int32_t;
	using oid_t = uint16_t;
}
