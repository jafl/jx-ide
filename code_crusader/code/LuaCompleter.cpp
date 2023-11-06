/******************************************************************************
 LuaCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 2002 by John Lindal.

 ******************************************************************************/

#include "LuaCompleter.h"
#include <jx-af/jcore/jAssert.h>

LuaCompleter* LuaCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// keywords

	"and", "break", "do", "else", "elseif", "end", "for", "function", "if",
	"in", "local", "nil", "not", "or", "repeat", "return", "then", "until",
	"while",

	// functions

	"_ALERT", "assert", "call", "collectgarbage", "copytagmethods", "dofile",
	"dostring", "error", "foreach", "foreachi", "getglobal", "getn",
	"gettagmethod", "globals", "newtag", "next", "print", "rawget", "setglobal",
	"settag", "settagmethod", "sort", "tag", "tonumber", "tostring", "tinsert",
	"tremove", "type", "strbyte", "strchar", "strfind", "strlen", "strlower",
	"strrep", "strsub", "strupper", "format", "gsub", "abs", "acos", "asin",
	"atan", "atan2", "ceil", "cos", "deg", "exp", "floor", "log", "log10",
	"max", "min", "mod", "rad", "sin", "sqrt", "tan", "frexp", "ldexp",
	"random", "randomseed", "openfile", "closefile", "readfrom", "writeto",
	"appendto", "remove", "rename", "flush", "seek", "set", "cur", "end",
	"tmpname", "read", "write", "clock", "date", "execute", "exit", "getenv",
	"setlocale",

	// debugging

	"getinfo", "getlocal", "setlocal", "setcallhook", "setlinehook"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
LuaCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew LuaCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
LuaCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

LuaCompleter::LuaCompleter()
	:
	StringCompleter(kLuaLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LuaCompleter::~LuaCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

bool
LuaCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_';
}
