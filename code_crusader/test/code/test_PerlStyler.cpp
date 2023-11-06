/******************************************************************************
 test_PerlStyler.cpp

	Test Perl styler.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "PerlStyler.h"
#include "testUtil.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

class TestPerlStyler : public PerlStyler
{
public:

	TestPerlStyler()
	{
		SetWordStyle(
			JString("#line", JString::kNoCopy),
			JFontStyle(true, false, 0, false, JColorManager::GetOrangeColor()));
	}
};

int main()
{
	return JTestManager::Execute();
}

JTEST(Basic)
{
	JStyledText* st = jnew JStyledText(false, false);

	TestPerlStyler styler;

	// empty
	UpdateStyles(st, &styler);

	RunTest(st, &styler, "./data/styler/perl/test.pl", "./data/styler/perl/styled.out");
}
