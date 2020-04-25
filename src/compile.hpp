#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tools.hpp"
#include <string>
#include <jsoncpp/json/json.h>
#include <iostream>
#include "oj_log.hpp"



class Compiler
{
    public:
        //有可能浏览器对不同的题目提交的数据不同
        static void CompilerAndRun(Json::Value req, Json::Value* resp)
        {
            if (req["code"].empty()) {
                LOG(ERROR, "Request code id empty") << std::endl;
                return;
            }

            //2.将代码写到文件中去
            std::string code = req["code"].asString();
            std::string tmp_filename = WriteTmpFile(code);
            if (tmp_filename == "") {
                LOG(ERROR,"Write source failed");
                return;
            }

            //对文件名称进行约定 tmp_时间戳
            
            //编译
            if (!Compile(tmp_filename)) {
                LOG(ERROR, "Compile Error") << std::endl;
                return;
            }
            //运行
            //构造响应

        }


    private:
        static std::string WriteTmpFile(const std::string& code)
        {
            //组织文件名称。组织文件的前缀名称，用来区分源码文件，可知性文件时一组数据
            std::string tmp_filename = "/tmp_" + std::to_string(LogTime::GetTimeStamp());
            //写文件
            int ret = FileOpen::WriteDataToFile(SrcPath(tmp_filename), code);
            if (ret < 0) {
                LOG(ERROR, "Write code to source failed");
                return "";
            }
            return tmp_filename;
        }

        static std::string SrcPath(const std::string& filename)
        {
            return "./tmp_files" + filename + ".cpp";
        }
        static std::string ExePath(const std::string& filename)
        {
            return "./tmp_files" + filename + ".executable";
        }
        static std::string ErrorPath(const std::string& filename)
        {
            return "./tmp_files" + filename + ".error";
        }
        
        static bool Compile(const std::string& filename)
        {
            //1.构造编译命令
            const int command_count = 20;
            char buf[command_count][50] = {{0}};
            char* command[command_count] = {0};
            for(int i = 0; i < command_count; ++i) {
                command[i] = buf[i];
            }
            snprintf(command[0], 49, "%s", "g++");
            snprintf(command[1], 49, "%s", SrcPath(filename).c_str());
            snprintf(command[2], 49, "%s", "-o");
            snprintf(command[3], 49, "%s", ExePath(filename).c_str());
            snprintf(command[4], 49, "%s", "-std=c++11");
            command[5] = NULL;
            //2.创建子进程 
            int pid = fork();
            if (pid < 0) {

                LOG(ERROR, "fork failed");
            }
            else if (pid == 0) {
            //2.2 子进程  程序替换
                int fd = open(ErrorPath(filename).c_str(), O_CREAT | O_RDWR, 0664);
                //重定向
                dup2(fd, 2);
                //程序替换
                execvp(command[0], command);
                exit(0);


            }
            else {
            //2.1 父进程
                waitpid(pid, NULL, 0);

            }
            //3.验证是否生成可执行程序

            return true;
        }

        static bool Run()
        {

            return true;
        }
};
