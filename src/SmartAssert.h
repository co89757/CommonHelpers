
#include <string>
#include <functional>
#include <sstream>
#include <valarray>
#include <stdio.h>
#include <stdlib.h>
#include <memory>

namespace colinli {
namespace utility {

/// <summary>
/// class that encapsulates all context information such as expression that
/// triggers the assertion failure,
/// local variables and log message;
/// </summary>
class AssertContext final {

 public:
  AssertContext() = default;
  //AssertContext(const std::string& expr, const std::string& file, int lineno);
  AssertContext(const AssertContext& other) = delete;
  AssertContext& operator=(const AssertContext& other) = delete;

  template <typename T> void addValue(T val, const std::string& varRepr) {
    variableRepr << varRepr << " = " << val << "\n";
  }

  // store context line number
  int Line;
  // stores context filename
  std::string FileName;
  // store log message
  std::string Message;
  //expression that triggers the assertion
  std::string Expression;

  std::string PrintContextVariables();

  std::stringstream& getVariableReprStream() { return variableRepr; }

 private:
  // stores context variables and their string representations
  std::stringstream variableRepr;

};

typedef enum {

  ASSERT_WARNING,
  ASSERT_DEBUG,
  ASSERT_ERROR,
  ASSERT_FATAL

} AssertLevel;

class Assert {
 private:
  Assert(const Assert& other) = delete;
  Assert& operator=(const Assert& other) = delete;

  Assert() {}
  static std::unique_ptr<Assert> pSingle;

 public:
  typedef std::function<void(AssertContext&)> HandlerType;

  static const int NUM_LEVELS = 4;

  static Assert& getSingleton() {
    if (pSingle)
      return *pSingle;
    else {
      pSingle.reset(new Assert());
      return *pSingle;
    }
  }

  ~Assert() {}

  Assert& SMART_ASSERT_A = *this;
  Assert& SMART_ASSERT_B = *this;
  Assert& CaptureExpression(const std::string& expr) {
    assertContext.Expression = expr;
    return *this;
  }
  Assert& CaptureFileLineContext(const std::string& file, int line) {
    assertContext.Line = line;
    assertContext.FileName = file;
    return *this;
  }
  Assert& LogMessage(const std::string& message) {
    assertContext.Message = message;
    return *this;
  }

  template <typename T>
  Assert& CaptureLocalVariable(T x, const std::string& xstr) {
    assertContext.addValue(x, xstr);
    return *this;
  }

  static void SetHandler(AssertLevel level, HandlerType _handler);

  void onLevelTriggered(AssertLevel level);

  void Error(const std::string& msg);

  void Debug(const std::string& msg);

  void Warning(const std::string& msg);

  void Fatal(const std::string& msg);

  static std::valarray<HandlerType> handlers;
  static HandlerType defaultHandler;

 private:
  AssertContext assertContext;

};

#define SMART_ASSERT_A(x) SMART_ASSERT_OP(x, B)
#define SMART_ASSERT_B(x) SMART_ASSERT_OP(x, A)
#define SMART_ASSERT_OP(x, next) \
  SMART_ASSERT_A.CaptureLocalVariable((x), #x).SMART_ASSERT_##next

#define ENSURES(expr)                                         \
  if(!(expr)) Assert::getSingleton().CaptureExpression(#expr) \
      .CaptureFileLineContext(__FILE__, __LINE__).SMART_ASSERT_A

}  // end of utility ns
}  // end of colinli ns