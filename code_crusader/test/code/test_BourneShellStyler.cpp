/******************************************************************************
 test_BourneShellStyler.cpp

	Test bash styler.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "BourneShellStyler.h"
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

	StylerBase* styler = BourneShellStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/bash/test.sh", "./data/styler/bash/styled.out");
}
