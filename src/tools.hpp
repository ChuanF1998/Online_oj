#pragma once

#include "oj_log.hpp"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <unordered_map>


//实现一个切割字符串的工具函数
class StringTools
{

    public:
        static void Split(const std::string& input, const std::string& split_char, std::vector<std::string>* output)
        {
            boost::split(*output, input, boost::is_any_of(split_char), boost::token_compress_off);
        }
};

//实现文件操作的类
class FileOpen
{
    public:
        static int ReadDataFromFile(std::string filename, std::string* content)
        {
            std::ifstream file(filename.c_str());
            if (!file.is_open()) {
                LOG(ERROR, "Open file failed! filename id") << filename << std::endl;
                return -1;
            }

            std::string line;
            while (std::getline(file, line)) {
                *content += line + "\n";
            }
            file.close();
            return 0;
        }
        

        static int WriteDataToFile(const std::string& filename, const std::string& Data) 
        {

            std::ofstream file(filename.c_str());
            if(!file.is_open()) {
                LOG(ERROR, "Open file failed") << filename << std::endl;
                return -1;
            }

            file.write(Data.data(), Data.size());
            file.close();
            return 0;
        }
};


class UrlUtil
{
    public:
        static void PraseBody(const std::string& body, std::unordered_map<std::string, std::string>* pram)
        {

            //name=xxx&stdin=xxx
            std::vector<std::string> tokens;
            StringTools::Split(body, "&", &tokens);
            for(const auto& token : tokens) {
                //name=xxx
                std::vector<std::string> vec;
                StringTools::Split(token, "=", &vec);
                if (vec.size() != 2) {
                    continue;
                }

                //url解码j
                (*pram)[vec[0]] = UrlDecode(vec[1]);

            }
        }

    private:
        static unsigned char ToHex(unsigned char x)   
        {   
            return  x > 9 ? x + 55 : x + 48;   
        }  

        static unsigned char FromHex(unsigned char x)   
        {   
            unsigned char y;  
            if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;  
            else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;  
            else if (x >= '0' && x <= '9') y = x - '0';  
            else assert(0);  
            return y;  
        }  

        static std::string UrlEncode(const std::string& str)  
        {  
            std::string strTemp = "";  
            size_t length = str.length();  
            for (size_t i = 0; i < length; i++)  
            {  
                if (isalnum((unsigned char)str[i]) ||   
                        (str[i] == '-') ||  
                        (str[i] == '_') ||   
                        (str[i] == '.') ||   
                        (str[i] == '~'))  
                    strTemp += str[i];  
                else if (str[i] == ' ')  
                    strTemp += "+";  
                else  
                {  
                    strTemp += '%';  
                    strTemp += ToHex((unsigned char)str[i] >> 4);  
                    strTemp += ToHex((unsigned char)str[i] % 16);  
                }  
            }  
            return strTemp;  
        }  

        static std::string UrlDecode(const std::string& str)  
        {  
            std::string strTemp = "";  
            size_t length = str.length();  
            for (size_t i = 0; i < length; i++)  
            {  
                if (str[i] == '+') strTemp += ' ';  
                else if (str[i] == '%')  
                {  
                    assert(i + 2 < length);  
                    unsigned char high = FromHex((unsigned char)str[++i]);  
                    unsigned char low = FromHex((unsigned char)str[++i]);  
                    strTemp += high*16 + low;  
                }  
                else strTemp += str[i];  
            }  
            return strTemp;  
        }
};
