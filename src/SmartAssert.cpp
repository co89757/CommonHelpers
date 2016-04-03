#include "SmartAssert.h"
#include <iostream>
namespace colinli {
namespace utility
{
	    std::unique_ptr<Assert> Assert::pSingle = nullptr;

		Assert::HandlerType Assert::defaultHandler = [&](AssertContext& ctx)
		{

			std::cout << ctx.Message << "\n";
			std::cout << "Expression: " << ctx.Expression << "\n" << "FILE: " << ctx.FileName << ":" << ctx.Line << "\n" <<
				"Local Variables: " << ctx.PrintContextVariables();
		};

		std::valarray<Assert::HandlerType> Assert::handlers(Assert::defaultHandler, Assert::NUM_LEVELS);

		void Assert::SetHandler(AssertLevel level, Assert::HandlerType _handler)
		{
			handlers[level] = _handler;
		}

		void Assert::onLevelTriggered(AssertLevel level)
		{
			handlers[level](assertContext);
		}

		void Assert::Error(const std::string& msg)
		{
			LogMessage(msg);
			onLevelTriggered(ASSERT_ERROR);
		}

		void Assert::Debug(const std::string& msg)
		{
			LogMessage(msg);
			onLevelTriggered(ASSERT_DEBUG);
		}

		void Assert::Warning(const std::string& msg)
		{
			LogMessage(msg);
			onLevelTriggered(ASSERT_WARNING);

		}

		void Assert::Fatal(const std::string& msg)
		{
			LogMessage(msg);
			onLevelTriggered(ASSERT_FATAL);

		}

		std::string AssertContext::PrintContextVariables()
		{
			auto a = variableRepr.str();
			variableRepr.str(std::string());
			return a;

		}


 }
}