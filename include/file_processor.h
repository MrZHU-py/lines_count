#pragma once

#include "file_stats.h"
#include "thread_pool.h"
#include <string>

// 处理单个文件，更新统计数据
void process_file(const std::string& file_path, FileStats& stats);

// 处理目录中的所有文件，将任务分配给线程池
void process_directory(const std::string& directory_path, FileStats& stats, ThreadPool& thread_pool);
