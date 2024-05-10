/******************************************************************************
 JSPCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#include "JSPCompleter.h"
#include "HTMLCompleter.h"
#include "JavaScriptCompleter.h"
#include "JavaScriptStyler.h"
#include "HTMLStyler.h"
#include <jx-af/jcore/jAssert.h>

JSPCompleter* JSPCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// remember to update JavaCompleter

	// keywords

	"abstract", "boolean", "break", "byte", "byvalue", "case", "cast",
	"catch", "char", "class", "const", "continue", "default", "do",
	"double", "else", "enum", "extends", "false", "final", "finally", "float",
	"for", "future", "generic", "goto", "if", "implements", "import",
	"inner", "instanceof", "int", "interface", "long", "native", "new",
	"null", "operator", "outer", "package", "private", "protected",
	"public", "rest", "return", "short", "static", "super", "switch",
	"synchronized", "this", "throw", "throws", "transient", "true",
	"try", "var", "void", "volatile", "while",

	// methods

	"clone", "equals", "finalize", "getClass", "hashCode", "notify",
	"notifyAll", "toString", "wait",

	// lang package

	"Appendable", "Boolean", "Byte", "Character", "CharSequence",
	"Class", "ClassLoader", "Cloneable", "Comparable", "Compiler",
	"Double", "Enum", "Float", "InheritableThreadLocal", "Integer",
	"Iterable", "Long", "Math", "Number", "Object", "Package",
	"Process", "ProcessBuilder", "Readable", "Runnable", "Runtime",
	"RuntimePermission", "SecurityManager", "Short", "StackTraceElement",
	"StrictMath", "String", "StringBuffer", "StringBuilder",
	"System", "Thread", "ThreadGroup", "ThreadLocal", "Throwable", "Void",

	// util package

	"AbstractCollection", "AbstractList", "AbstractMap", "AbstractQueue",
	"AbstractSequentialList", "AbstractSet", "ArrayList", "Arrays", "BitSet",
	"Calendar", "Collections", "Comparator", "Currency", "Date", "Dictionary",
	"EnumMap", "EnumSet", "Enumeration", "EventListenerProxy", "EventObject",
	"FormattableFlags", "Formatter", "GregorianCalendar", "HashMap", "HashSet",
	"Hashtable", "IdentityHashMap", "Iterator", "LinkedHashMap", "LinkedHashSet",
	"LinkedList", "List", "ListIterator", "ListResourceBundle", "Locale",
	"Map", "Map.Entry", "Observable", "Observer", "PriorityQueue", "Properties",
	"PropertyPermission", "PropertyResourceBundle", "Queue", "Random",
	"RandomAccess", "ResourceBundle", "Scanner", "Set", "SimpleTimeZone",
	"SortedMap", "SortedSet", "Stack", "StringTokenizer", "Timer", "TimerTask",
	"TimeZone", "TreeMap", "TreeSet", "UUID", "Vector", "WeakHashMap"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
JSPCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew JSPCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
JSPCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JSPCompleter::JSPCompleter()
	:
	StringCompleter(kJSPLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
	UpdateWordListExtra();	// include HTML and JavaScript
	ListenTo(HTMLStyler::Instance());
	ListenTo(JavaScriptStyler::Instance());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSPCompleter::~JSPCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
JSPCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == '.');
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JSPCompleter::Receive
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
JSPCompleter::UpdateWordListExtra()
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
