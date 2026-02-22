#pragma once
#include "BS_thread_pool.hpp"
#include "coro/thread_pool.hpp"
inline BS::thread_pool<BS::tp::none> threadPool{};
inline BS::thread_pool<BS::tp::none> scanThreadPool{250};
inline BS::thread_pool<BS::tp::none> metaDataThreadPool{250};
inline bool stopScanning = false;