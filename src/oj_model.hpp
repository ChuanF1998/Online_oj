#pragma once
#include "oj_log.hpp"
#include "tools.hpp"
#include <fstream>
#include <unordered_map>
#include <string>
#include <iostream>
#include <algorithm>

typedef struct Question
{

    std::string _id;
    std::string _name;
    std::string _path;
    std::string _star;
}Ques;


class OjModel
{
    public:

        OjModel()
        {
            LoadQuestions("./config_oj.cfg");
        }
        bool GetAllQuestions(std::vector<Question>* ques) {
            for (const auto& kv : _model_map) {
                ques->push_back(kv.second);
            }
            std::sort(ques->begin(), ques->end(), [](const Question& le, const Question& ri){
                    return std::atoi(le._id.c_str()) < std::atoi(ri._id.c_str());
                    });
            return true;
        }


        bool GetOneQuestion(const std::string& id, std::string* desc, std::string* header, Question* ques)
        {
            //1.根据id去查找对应题目信息，在哪里加载
            auto iter = _model_map.find(id);
            if (iter == _model_map.end()) {
                LOG(ERROR, "Not Found Question id is") << " " << id << std::endl;
                return false;
            }

            *ques = iter->second;

            //iter->second._path;
            //加载具体的单个题目信息，从保存的路径上面去加载
            //从具体的题目文件当中去获取两部分信息，描述，header
            int ret = FileOpen::ReadDataFromFile(DescPath(iter->second._path), desc);
            if (ret == -1) {
                LOG(ERROR, "Read desc failed") << std::endl;
            }
            ret = FileOpen::ReadDataFromFile(HeaderPath(iter->second._path), header);
            if (ret == -1) {
                LOG(ERROR, "Header desc failed") << std::endl;
            }
            return true;
        }


        bool SplitingCode(std::string user_code, std::string ques_id, std::string* code)
        {
            //1.查找以下对应id的题目是否存在
            auto iter = _model_map.find(ques_id);
            if (iter == _model_map.end()) {
                LOG(ERROR, "can not find questions id is") << ques_id << std::endl;
                return false;
            }

            std::string tail_code;
            int ret = FileOpen::ReadDataFromFile(TailPath(iter->second._path), &tail_code);
            if (ret < 0) {
                LOG(ERROR, "Open tail.cpp failed") << std::endl;
                return false;
            }
            *code = user_code + tail_code;
            return true;
        }
    private:
        std::string DescPath(const std::string& que_path)
        {
            return que_path + "desc.txt";
        }
        std::string HeaderPath(const std::string& que_path)
        {
            return que_path + "header.cpp";
        }
        std::string TailPath(const std::string& que_path)
        {
            return que_path + "tail.cpp";
        }

        bool LoadQuestions(const std::string& configfile_path)
        {
            //使用C++当中的文件流来加载文件
            //iostream:处理控制台
            //fstream：处理命名文件
            //stringstream：处理string
            //ostream：output文件流
            //istream：input文件流，从文件当中读

            std::ifstream file(configfile_path.c_str());
            if(!file.is_open()){
                return false;
            }

            std::string line;
            while(std::getline(file, line)) {
                //切割字符串
                std::vector<std::string> vec;
                StringTools::Split(line, " ", &vec);
                if (vec.size() != 4) {
                    continue;
                }
                //将切割后的字符放到unordered_map
                Question ques;
                ques._id = vec[0];
                ques._name = vec[1];
                ques._path = vec[2];
                ques._star = vec[3];
                _model_map[ques._id] = ques;
            }
            file.close();
            return true;
        }

    private:
        //试题id，名称，路径，难度
        std::unordered_map<std::string, Question> _model_map;

};
