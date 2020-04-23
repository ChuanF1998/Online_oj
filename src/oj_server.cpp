#include "httplib.h"

#include "oj_model.hpp"


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
          //想使用模板技术去填充html页面
      resp.set_content("<html></html>", "text/html");
      });
  svr.listen("0.0.0.0", 20000);
  return 0;
}
