#include "SmartAssert.h"
#include <iostream>
#include "TimeUtils.h"

using std::cout;
using namespace colinli::utility;
void TestAssert()
{
	int i = 0;
	Assert::HandlerType debugf = [&](AssertContext& ctx)
	{
		cout << "[DEBUG] Description: " << ctx.Message << "\n";
		cout << "Expression: " << ctx.Expression << "\n" << "FILE: " << ctx.FileName << ":" << ctx.Line << "\n" <<
			"Local Variables: " << ctx.PrintContextVariables() << "\n";
	};
	int j = 1;
	Assert::SetHandler(DEBUG, debugf);
	ENSURES(i < 0)(i).Debug("integer is negative");
	ENSURES(j > 3)(j).Debug("integer is not bigger than 3");
	system("pause");
}

void TestTime()
{
	cout<< time::PrintLocalTime();
}


int main()
{
	// TestAssert();
	TestTime();
    
}