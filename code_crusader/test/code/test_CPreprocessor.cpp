/******************************************************************************
 test_CPreprocessor.cpp

	Test C preprocessor.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "CPreprocessor.h"
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Basic)
{
	CPreprocessor cpp;

	const JString s("abcdFOObar\nabcd FOO bar", JString::kNoCopy);

	JString text = s ;
	cpp.Preprocess(&text);
	JAssertStringsEqual(s, text);

	cpp.DefineMacro(JString("FOO", JString::kNoCopy), JString("BAR", JString::kNoCopy));

	cpp.Preprocess(&text);
	JAssertStringsEqual("abcdFOObar\nabcd BAR bar", text);

	cpp.UndefineAllMacros();

	text = s;
	cpp.Preprocess(&text);
	JAssertStringsEqual(s, text);
}
