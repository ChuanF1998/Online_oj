#include "httplib.h"

#include "oj_model.hpp"
#include "oj_view.hpp"
#include "oj_log.hpp"
#include "compile.hpp"


int main()
{
    //httplib的时候，需要使用httplib使用的命名空间
    using namespace httplib;

    Server svr;

    OjModel oj_model;
    //获取试题信息，信息来源于文件
    svr.Get("/all_questions", [&oj_model](const Request& req, Response& resp){
            std::vector<Question> ques;
            oj_model.GetAllQuestions(&ques);
            //    std::cout << ques.size() << std::endl;
            //char buf[10240] = {'\0'};
            //snprintf(buf, sizeof(buf) - 1, "<html>%s.%s %s</html>", ques[0]._id.c_str(), ques[0]._name.c_str(), ques[0]._star.c_str());
            std::string html;
            //html.assign(buf, strlen(buf));
            //想使用模板技术去填充html页面
            OjView::ExpandAllQuestionsHtml(&html, ques);
            //    LOG(INFO, html);
            resp.set_content(html, "text/html; charset=UTF-8");
            });

    //正则表达式
    // \b 单词的分界
    // *：匹配任意字符串
    // \d:匹配一个数字
    // (): 分组应用
    // 源码转义：特殊字符按照特殊字符字面源码编译 R"(str")
  svr.Get(R"(/question/(\d+))", [&oj_model](const Request& req, Response& resp){
      //1.去试题模块查找对应题号的具体题目信息
      //  map当中（序号 名称 题目地址 难度）

      std::string desc;
      std::string header;
      //从querystr中获取id
      LOG(INFO, "req.matches") << req.matches[0] << ":" << req.matches[1] << std::endl;
      //  2. 在题目地址的路径下去加载单个题目的描述信息
      struct Question ques;
      oj_model.GetOneQuestion(req.matches[1].str(), &desc, &header, &ques);
      //3.组织，返回给浏览器
      std::string html;
      OjView::ExpandOneQuestion(ques, desc, header, &html);
      resp.set_content(html, "text/html; charset=UTF-8");
  });

  svr.Post(R"(/question/(\d+))", [&oj_model](const Request& req, Response& resp){
          //1.从正文当中提取出来提交的内容，主要是提取code字段的内容
          // 提交的内容当中有url编码，需要对提交内容解码
          std::unordered_map<std::string, std::string> pram;
          UrlUtil::PraseBody(req.body, &pram);
          //for (const auto& pr:pram) {
            //  LOG(INFO, "code ") << pr.second << std::endl;
         // }
          //  2. 编译&&运行
          std::string code;
          oj_model.SplitingCode(pram["code"], req.matches[1].str(), &code);
          //LOG(INFO, "code") << code << std::endl;
          Json::Value req_json;
          req_json["code"] = code;
          Json::Value resp_json;
          Compiler::CompilerAndRun(req_json, &resp_json);
          //  3.构造响应，json
          const std::string errorno = resp_json["errorno"].asString();
          const std::string reason = resp_json["reason"].asString();
          const std::string stdout_reason = resp_json["stdout"].asString();
          std::string html;
          OjView::ExpandReason(errorno, reason, stdout_reason, &html);
          resp.set_content(html, "text/html; charset=UTF-8");
          });
  LOG(INFO, "Listen in 0.0.0.0:19999");
  LOG(INFO, "Server ready");
  svr.listen("0.0.0.0", 20000);
  return 0;
}
