#pragma once

#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tools.hpp"
#include <string>
#include <jsoncpp/json/json.h>
#include <iostream>
#include "oj_log.hpp"


enum ErrorNo
{
    OK = 0,
    COMPILE_ERROR,
    RUN_ERROR,
    PRAM_ERROR,
    INTER_ERROR
};

class Compiler
{
    public:
        //有可能浏览器对不同的题目提交的数据不同
        //0：编译错误   2：运行错误  3：参数错误  4：内部错误
        static void CompilerAndRun(Json::Value req, Json::Value* resp)
        {
            if (req["code"].empty()) {
                (*resp)["errorno"] = PRAM_ERROR;
                (*resp)["reason"] = "Pram error";
                LOG(ERROR, "Request code id empty") << std::endl;
                return;
            }

            //2.将代码写到文件中去
            std::string code = req["code"].asString();
            std::string tmp_filename = WriteTmpFile(code);
            if (tmp_filename == "") {
                (*resp)["errorno"] = INTER_ERROR;
                (*resp)["reason"] = "Create file failed";
                LOG(ERROR,"Write source failed");
                return;
            }

            //对文件名称进行约定 tmp_时间戳

            //编译
            if (!Compile(tmp_filename)) {
                (*resp)["errorno"] = COMPILE_ERROR;
                std::string reason;
                FileOpen::ReadDataFromFile(ErrorPath(tmp_filename), &reason);
                (*resp)["reason"] = reason; 
                LOG(ERROR, "Compile Error") << std::endl;
                return;
            }
            //运行
            int sig = Run(tmp_filename);
            if (sig != 0) {
                (*resp)["errorno"] = RUN_ERROR;
                (*resp)["reason"] = "Program exit by sig" + std::to_string(sig);
                LOG(ERROR, "run error") << std::endl;
                return;

            }
            //构造响应
            (*resp)["errorno"] = OK;
            (*resp)["reason"] = "Compile and run is ok!";

            //标准输出
            std::string stdout_reason;
            FileOpen::ReadDataFromFile(StdoutPath(tmp_filename), &stdout_reason);
            (*resp)["stdout"] = stdout_reason;
            //标准错误
            std::string stderr_reason;
            FileOpen::ReadDataFromFile(StderrPath(tmp_filename), &stderr_reason);
            (*resp)["stdout"] = stderr_reason;


            //清理临时文件
            Clean(tmp_filename);
            return;

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
        static std::string StdoutPath(const std::string& filename)
        {
            return "./tmp_files" + filename + ".stdout";
        }
        static std::string StderrPath(const std::string& filename)
        {
            return "./tmp_files" + filename + ".stderr";
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
            snprintf(command[5], 49, "%s", "-D");
            snprintf(command[6], 49, "%s", "CompileOnline");
            command[7] = NULL;
            //2.创建子进程 
            int pid = fork();
            if (pid < 0) {

                LOG(ERROR, "fork failed");
            }
            else if (pid == 0) {
                //2.2 子进程  程序替换
                int fd = open(ErrorPath(filename).c_str(), O_CREAT | O_RDWR, 0664);
                if (fd < 0) {
                    LOG(ERROR, "Open Compile failed") << std::endl;
                    exit(1);
                }
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

            struct stat st;
            int ret = stat(ExePath(filename).c_str(), &st);
            if (ret < 0) {
                LOG(ERROR, "Compile ERROR! exe filename is ") << ExePath(filename) << std::endl;
                return false;
            }

            return true;
        }

        static int Run(const std::string& filename)
        {

            //可执行程序
            //1.创建子进程
            int pid = fork();
            if (pid < 0) {
                LOG(ERROR, "Exec pragma failed") << std::endl;
                return -1;
            }
            else if (pid == 0) {
                //对于子进程执行的限制
                //1.时间限制
                alarm(1);
                //2.内存限制
                struct rlimit rl;
                rl.rlim_cur = 1024 * 30000;
                rl.rlim_max = RLIM_INFINITY; //无限制
                setrlimit(RLIMIT_AS, &rl);

                //标准输出重定义到文件
                int stdout_fd = open(StdoutPath(filename).c_str(), O_CREAT | O_RDWR, 0664);
                if (stdout_fd < 0) {
                    LOG(ERROR, "Open stdout file failed") << StdoutPath(filename) << std::endl;
                    return -1;
                }
                dup2(stdout_fd, 1);

                //标准错误重定向到文件
                int stderr_fd = open(StdoutPath(filename).c_str(), O_CREAT | O_RDWR, 0664);
                if (stderr_fd < 0) {
                    LOG(ERROR, "Open stderr file failed") << StdoutPath(filename) << std::endl;
                    return -1;
                }
                dup2(stdout_fd, 2);

                execl(ExePath(filename).c_str(), ExePath(filename).c_str(), NULL);
                exit(1);
            }
            int status = -1;
            waitpid(pid, &status, 0);
            //将是否收到信号的信息返回给调用者，如果调用者判断为0，
            return status & 0x7f;

        }
        

        static void Clean(std::string filename)
        {
            unlink(SrcPath(filename).c_str());
            unlink(ExePath(filename).c_str());
            unlink(ErrorPath(filename).c_str());
            unlink(StdoutPath(filename).c_str());
            unlink(StderrPath(filename).c_str());
        }
};
