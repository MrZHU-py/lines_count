#pragma once

#include <atomic>

struct FileStats 
{
    // 代码行数
    std::atomic<int> code_lines{0};
    
    // 注释行数
    std::atomic<int> comment_lines{0};
    
    // 空白行数
    std::atomic<int> blank_lines{0};
    
    // 文件计数
    std::atomic<int> file_count{0};
};
