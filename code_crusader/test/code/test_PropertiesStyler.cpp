/******************************************************************************
 test_PropertiesStyler.cpp

	Test Properties styler.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "PropertiesStyler.h"
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

	StylerBase* styler = PropertiesStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/properties/test.properties", "./data/styler/properties/styled.out");
}
