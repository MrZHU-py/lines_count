#include "file_processor.h"
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <cstring>

std::mutex queue_mutex;              // 互斥量，用于保护文件队列
std::queue<std::string> file_queue; // 文件队列

// 判断路径是否为常规文件
bool is_regular_file(const std::string& path) 
{
    struct stat s;
    if (stat(path.c_str(), &s) == 0) 
    {
        return S_ISREG(s.st_mode);
    }
    return false;
}

// 检查文件路径是否具有指定的扩展名
bool has_extension(const std::string& path, const std::string& ext) 
{
    return path.size() >= ext.size() && 
           path.compare(path.size() - ext.size(), ext.size(), ext) == 0;
}

// 处理单个文件，统计代码行、注释行和空白行
void process_file(const std::string& file_path, FileStats& stats) 
{
    std::ifstream file(file_path);
    if (!file.is_open()) 
    {
        std::cerr << "无法打开文件: " << file_path << std::endl;
        return;
    }

    std::string line;
    bool in_multiline_comment = false;
    while (std::getline(file, line)) 
    {
        // 去除行首和行尾的空白字符
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty()) 
        {
            stats.blank_lines++;
        } 
        else if (in_multiline_comment) 
        {
            stats.comment_lines++;
            if (line.find("*/") != std::string::npos) 
            {
                in_multiline_comment = false;
            }
        } 
        else if (line.substr(0, 2) == "//") 
        {
            stats.comment_lines++;
        } 
        else if (line.find("/*") != std::string::npos) 
        {
            stats.comment_lines++;
            if (line.find("*/") == std::string::npos) 
            {
                in_multiline_comment = true;
            }
        } 
        else 
        {
            stats.code_lines++;
        }
    }
}

// 处理目录及其子目录，收集文件并将其推入队列
void process_directory(const std::string& directory_path, FileStats& stats, ThreadPool& thread_pool) 
{
    DIR* dir = opendir(directory_path.c_str());
    if (!dir) 
    {
        std::cerr << "无法打开目录: " << directory_path << std::endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) 
    {
        std::string path = directory_path + "/" + entry->d_name;

        if (entry->d_type == DT_DIR) 
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) 
            {
                process_directory(path, stats, thread_pool);
            }
        } 
        else if (is_regular_file(path) && (has_extension(path, ".c") || has_extension(path, ".h"))) 
        {
            file_queue.push(path);
        }
    }

    closedir(dir);

    // 使用线程池处理文件队列中的文件
    while (!file_queue.empty()) 
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (!file_queue.empty()) 
        {
            std::string file_path = file_queue.front();
            file_queue.pop();
            thread_pool.enqueue([file_path, &stats]() 
            {
                process_file(file_path, stats);
                stats.file_count++;
            });
        }
    }
}
