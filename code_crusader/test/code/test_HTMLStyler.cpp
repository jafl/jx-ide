/******************************************************************************
 test_HTMLStyler.cpp

	Test html/php/jsp/hbs styler.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "HTMLStyler.h"
#include "testUtil.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

class TestHTMLStyler : public HTMLStyler
{
public:

	TestHTMLStyler() {};

	void TestSetWordStyle(const JString& word, const JFontStyle& style);
};

void
TestHTMLStyler::TestSetWordStyle
	(
	const JString&		word,
	const JFontStyle&	style
	)
{
	SetWordStyle(word, style);
}

int main()
{
	return JTestManager::Execute();
}

JTEST(html)
{
	JStyledText* st = jnew JStyledText(false, false);

	TestHTMLStyler styler;

	// empty
	UpdateStyles(st, &styler);

	RunTest(st, &styler, "./data/styler/html/test.html", "./data/styler/html/styled-html.out");
}

JTEST(hbs)
{
	JStyledText* st = jnew JStyledText(false, false);

	TestHTMLStyler styler;
	RunTest(st, &styler, "./data/styler/html/test.hbs", "./data/styler/html/styled-hbs.out");
}

JTEST(php)
{
	JStyledText* st = jnew JStyledText(false, false);

	TestHTMLStyler styler;
	RunTest(st, &styler, "./data/styler/html/test.php", "./data/styler/html/styled-php.out");
}

JTEST(jsp)
{
	JStyledText* st = jnew JStyledText(false, false);

	TestHTMLStyler styler;
	styler.TestSetWordStyle("/form", JFontStyle(true, false, 0, false, JColorManager::GetBlueColor()));
	styler.TestSetWordStyle("set", JFontStyle(JColorManager::GetOrangeColor()));
	styler.TestSetWordStyle("x:choose", JFontStyle(true, false, 0, false));

	RunTest(st, &styler, "./data/styler/html/test.jsp", "./data/styler/html/styled-jsp.out");
}
