/******************************************************************************
 test_SQLStyler.cpp

	Test SQL styler.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "CBSQLStyler.h"
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

	CBStylerBase* styler = CBSQLStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/sql/test.ddl", "./data/styler/sql/styled.out");
}
