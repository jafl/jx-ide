/******************************************************************************
 JavaScriptCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 2006 by John Lindal.

 ******************************************************************************/

#include "JavaScriptCompleter.h"
#include <jx-af/jcore/jAssert.h>

JavaScriptCompleter* JavaScriptCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// keywords

	"abstract", "arguments", "async", "await", "boolean", "break", "byte",
	"case", "catch", "char", "class", "const", "constructor", "continue",
	"debugger", "default", "delete", "do", "double", "else", "enum",
	"eval", "export", "extends", "false", "final", "finally", "float",
	"for", "function", "goto", "if", "implements", "import", "in",
	"instanceof", "int", "interface", "label", "let", "long", "native", "new",
	"null", "of", "package", "private", "protected", "public", "return",
	"short", "static", "super", "switch", "synchronized", "this", "throw",
	"throws", "transient", "true", "try", "typeof", "var", "void",
	"volatile", "while", "with", "yield",

	// methods, classes, etc.

	"alert", "anchor", "anchors", "applet", "applets", "area", "arguments",
	"array", "assign", "blur", "body", "button", "callee", "caller",
	"captureEvents", "checkbox", "clearInterval", "clearTimeout", "close",
	"closed", "confirm", "date", "defaultStatus", "document", "element",
	"Error", "escape", "EvalError", "fileUpload", "find", "focus", "form",
	"forms", "frame", "frames", "function", "getClass", "hasOwnProperty",
	"hidden", "history", "home", "image", "images", "infinity",
	"innerHeight", "innerWidth", "isFinite", "isNaN", "isPrototypeOf",
	"java", "JavaArray", "JavaClass", "JavaObject", "JavaPackage",
	"length", "link", "links", "location", "LocationBar", "math",
	"MenuBar", "MimeType", "mimeTypes", "moveBy", "moveTo", "name", "NaN",
	"navigate", "Navigator", "netscape", "Number", "Object", "onblur",
	"onerror", "onfocus", "onload", "onunload", "open", "opener", "Option",
	"options", "outerHeight", "outerWidth", "Packages", "pageXOffset",
	"pageYOffset", "parent", "parseFloat", "parseInt", "password",
	"personalbar", "plugin", "plugins", "print", "prompt",
	"propertyIsEnum", "prototype", "Radio", "RangeError", "ReferenceError",
	"ref", "RegExp", "releaseEvents", "reset", "resizeBy", "resizeTo",
	"routeEvent", "screen", "script", "scroll", "scrollbars", "scrollBy",
	"scrollTo", "select", "self", "setTimeout", "status", "StatusBar",
	"stop", "String", "submit", "sun", "SyntaxError", "taint", "text",
	"TextArea", "Toolbar", "top", "toString", "TypeError", "undefined",
	"unescape", "untaint", "unwatch", "URIError", "valueOf", "watch",
	"window"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
JavaScriptCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew JavaScriptCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
JavaScriptCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JavaScriptCompleter::JavaScriptCompleter()
	:
	StringCompleter(kJavaScriptLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JavaScriptCompleter::~JavaScriptCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
JavaScriptCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == '.');
}

/******************************************************************************
 MatchCase (virtual protected)

 ******************************************************************************/

void
JavaScriptCompleter::MatchCase
	(
	const JString&	source,
	JString*		target
	)
	const
{
	target->ToLower();
	target->MatchCase(source, JCharacterRange(1, source.GetCharacterCount()));
}

/******************************************************************************
 GetDefaultWordList (static)

	Must be static because may be called before object is created.

 ******************************************************************************/

JSize
JavaScriptCompleter::GetDefaultWordList
	(
	const JUtf8Byte*** list
	)
{
	*list = kKeywordList;
	return kKeywordCount;
}
