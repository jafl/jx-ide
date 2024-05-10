/******************************************************************************
 PHPCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "PHPCompleter.h"
#include "HTMLCompleter.h"
#include "JavaScriptCompleter.h"
#include "JavaScriptStyler.h"
#include "HTMLStyler.h"
#include <jx-af/jcore/jAssert.h>

PHPCompleter* PHPCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	#include "pHPFunctionList.h"

	"__FILE__", "__LINE__", "__FUNCTION__", "__CLASS__", "__METHOD__",
	"abstract", "and", "array", "as",
	"bool", "boolean", "break",
	"case", "catch", "cfunction", "class", "clone", "const", "continue",
	"declare", "default", "do", "double",
	"echo", "else", "elseif", "enddeclare", "endif", "endfor", "endforeach",
	"endswitch", "endwhile", "exception", "extends",
	"if", "implements", "int", "integer", "interface",
	"false", "FALSE", "final", "float", "for", "foreach", "function",
	"global",
	"new", "null", "nullptr",
	"object", "old_function", "or",
	"parent", "php_user_filter", "print", "private", "protected", "public",
	"real", "return",
	"static", "string", "switch",
	"this", "throw", "true", "TRUE", "try",
	"use",
	"var",
	"while",
	"xor"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
PHPCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew PHPCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
PHPCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

PHPCompleter::PHPCompleter()
	:
	StringCompleter(kPHPLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
	UpdateWordListExtra();	// include HTML and JavaScript
	ListenTo(HTMLStyler::Instance());
	ListenTo(JavaScriptStyler::Instance());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PHPCompleter::~PHPCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (::) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
PHPCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == ':');
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
PHPCompleter::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if ((sender == HTMLStyler::Instance() ||
		 sender == JavaScriptStyler::Instance()) &&
		message.Is(StylerBase::kWordListChanged))
	{
		UpdateWordList();
	}
	else
	{
		StringCompleter::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateWordListExtra (virtual protected)

 ******************************************************************************/

void
PHPCompleter::UpdateWordListExtra()
{
	// include HTML words

	const JUtf8Byte** htmlWordList;
	JSize count = HTMLCompleter::GetDefaultWordList(&htmlWordList);
	for (JUnsignedOffset i=0; i<count; i++)
	{
		Add(JString(htmlWordList[i], JString::kNoCopy));
	}

	CopyWordsFromStyler(HTMLStyler::Instance());

	// include JavaScript words

	const JUtf8Byte** jsWordList;
	count = JavaScriptCompleter::GetDefaultWordList(&jsWordList);
	for (JUnsignedOffset i=0; i<count; i++)
	{
		Add(JString(jsWordList[i], JString::kNoCopy));
	}

	CopyWordsFromStyler(JavaScriptStyler::Instance());
	CopySymbolsForLanguage(kJavaScriptLang);
}
