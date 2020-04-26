#pragma once


#include "ctemplate/template.h"
#include <string>
#include <vector>
#include "oj_model.hpp"


class OjView
{

    public:
        //渲染html页面，并且返回页面·
        static void ExpandAllQuestionsHtml(std::string* html, std::vector<Question>& ques) {
            //获取数据字典-》将拿到的试题数据按照一定顺序保存到内存中
            ctemplate::TemplateDictionary dict("all_questions");

            for (const auto& que: ques) {
                ctemplate::TemplateDictionary* section_dict = dict.AddSectionDictionary("question");
               section_dict->SetValue("id", que._id);
               section_dict->SetValue("id", que._id);
               section_dict->SetValue("name", que._name);
               section_dict->SetValue("star", que._star);
            }
            //2.获取模板类的类指针，加载预定义的html页面到内存中
            ctemplate::Template* tl = ctemplate::Template::GetTemplate("./template/all_questions.html", ctemplate::DO_NOT_STRIP);
            //3.渲染，拿着模板类的指针，将数据字典当中的数据更新到html页面中

            tl->Expand(html, &dict);
        }


        //id name star desc header
        static void ExpandOneQuestion(const Question ques, std::string& desc, std::string& header, std::string* html)
        {
            ctemplate::TemplateDictionary dict("question");
            dict.SetValue("id", ques._id);
            dict.SetValue("name", ques._name);
            dict.SetValue("star", ques._star);
            dict.SetValue("desc", desc);
            dict.SetValue("header", header);
            ctemplate::Template* tpl = ctemplate::Template::GetTemplate("./template/question.html", ctemplate::DO_NOT_STRIP);
            tpl->Expand(html, &dict);
        }


        static void ExpandReason(const std::string& errorno, const std::string& reason, const std::string& stdout_reason, std::string* html)
        {

            ctemplate::TemplateDictionary dict("reason");
            dict.SetValue("errono", errorno);
            dict.SetValue("reason", reason);
            dict.SetValue("stdout", stdout_reason);
            ctemplate::Template* tpl = ctemplate::Template::GetTemplate("./template/reason.html", ctemplate::DO_NOT_STRIP);
            tpl->Expand(html, &dict);
        }
};
