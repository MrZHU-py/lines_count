#include <iostream>
#include <string>
#include "file_stats.h"
#include "file_processor.h"
#include "thread_pool.h"

int main(int argc, char* argv[]) 
{
    // 检查参数数量
    if (argc != 2) 
    {
        std::cerr << "缺少参数\n使用方式: " << argv[0] << " <directory>\n";
        return 1;
    }

    FileStats stats; 
    // 创建线程池，线程数为硬件并发数
    ThreadPool thread_pool(std::thread::hardware_concurrency());

    // 处理目录中的文件
    process_directory(argv[1], stats, thread_pool);

    // 等待所有线程完成任务
    thread_pool.wait_for_completion();

    int total_lines = stats.code_lines + stats.comment_lines + stats.blank_lines;

    // 输出统计结果
    std::cout << "总计处理文件数: " << stats.file_count << "\n";
    std::cout << "总行数: " << total_lines << "\n";
    std::cout << "总计代码行数: " << stats.code_lines << " (" << (double)stats.code_lines / total_lines * 100 << "%)\n";
    std::cout << "总计注释行数: " << stats.comment_lines << " (" << (double)stats.comment_lines / total_lines * 100 << "%)\n";
    std::cout << "总计空白行数: " << stats.blank_lines << " (" << (double)stats.blank_lines / total_lines * 100 << "%)\n";

    return 0;
}
