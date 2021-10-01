/******************************************************************************
 test_CSharpStyler.cpp

	Test C# styler.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "CSharpStyler.h"
#include "testUtil.h"
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Basic)
{
	JStyledText* st = jnew JStyledText(false, false);
	assert( st != nullptr );

	StylerBase* styler = CSharpStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/c#/test.cs", "./data/styler/c#/styled.out");
}
