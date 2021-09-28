/******************************************************************************
 test_PythonStyler.cpp

	Test Python styler.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "CBPythonStyler.h"
#include "cbTestUtil.h"
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Basic)
{
	JStyledText* st = jnew JStyledText(false, false);
	assert( st != nullptr );

	CBStylerBase* styler = CBPythonStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/python/test.py", "./data/styler/python/styled.out");
}
