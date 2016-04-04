#include "SmartAssert.h"
#include <iostream>
#include "TimeUtils.h"
#include "Logging.h"

using std::cout;
using namespace colinli::utility;
void TestAssert() {
  int i = 0;
  Assert::HandlerType debugf = [&](AssertContext & ctx) {
    cout << "[DEBUG] Description: " << ctx.Message << "\n";
    cout << "Expression: " << ctx.Expression << "\n"
         << "FILE: " << ctx.FileName << ":" << ctx.Line << "\n"
         << "Local Variables: " << ctx.PrintContextVariables() << "\n";
  }
  ;
  int j = 1;
  Assert::SetHandler(ASSERT_DEBUG, debugf);
  ENSURES(i < 0)(i).Debug("integer is negative");
  ENSURES(j > 3)(j).Debug("integer is not bigger than 3");
  // system("pause");
}

void TestTime() { cout << time::LocalTimeToString(); }

void testLog() {

  logging::Logging& logger = logging::Logging::GetLogger();

  logger.AddFileHandler("./testlog.log");
  LOG_SET_LEVEL(DEBUG);

  logger.AddStreamHandler(stderr);

  LOG(INFO) << "hello world on INFO level \n";
  const char* greet = "greetings from colin";
  int x = 42;
  LOGF(DEBUG, "GREETING: %s", greet);
  LOGF(DEBUG, "The answer to secrete of life: %d\n", x);

}

int main() {
  try {
    testLog();
    //	TestTime();
  }
  catch (std::runtime_error & ex) {
    printf("ERROR: %s \n", ex.what());
  }
}
