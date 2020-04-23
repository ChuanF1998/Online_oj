#pragma once
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

    private:
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
                StringTools::Split(line, "    ", &vec);
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
