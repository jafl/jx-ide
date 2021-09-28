/******************************************************************************
 test_PropertiesStyler.cpp

	Test Properties styler.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "CBPropertiesStyler.h"
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

	CBStylerBase* styler = CBPropertiesStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/properties/test.properties", "./data/styler/properties/styled.out");
}
