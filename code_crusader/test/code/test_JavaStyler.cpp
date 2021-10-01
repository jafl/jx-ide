/******************************************************************************
 test_JavaStyler.cpp

	Test Java styler.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "JavaStyler.h"
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

	StylerBase* styler = JavaStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/java/test.java", "./data/styler/java/styled.out");
}
