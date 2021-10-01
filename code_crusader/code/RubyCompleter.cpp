/******************************************************************************
 RubyCompleter.cpp

	Reference:  http://www.ruby-lang.org/

	BASE CLASS = StringCompleter

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "RubyCompleter.h"
#include <jx-af/jcore/jAssert.h>

RubyCompleter* RubyCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	"BEGIN", "END",
	"alias", "and",
	"begin", "break",
	"case", "class",
	"def", "defined", "do",
	"else", "elsif", "end", "ensure",
	"false", "for",
	"if", "in",
	"module",
	"next", "nil", "not",
	"or",
	"redo", "rescue", "retry", "return",
	"self", "super",
	"then", "true",
	"undef", "unless", "until",
	"when", "while",
	"yield",

	"Array", "Float", "Integer", "String",
	"at_exit", "autoload",
	"binding",
	"caller", "catch", "chop", "chomp",
	"eval", "exec", "exit",
	"fail", "fork", "format",
	"gets", "global_variables", "gsub",
	"iterator",
	"lambda", "load", "local_variables", "loop",
	"open",
	"p", "print", "printf", "proc", "putc", "puts",
	"raise", "rand", "readline", "readlines", "require",
	"select", "sleep", "split", "sprintf", "srand", "sub", "syscall", "system",
	"test", "trace_var", "trap",
	"untrace_var"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
RubyCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew RubyCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
RubyCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

RubyCompleter::RubyCompleter()
	:
	StringCompleter(kRubyLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RubyCompleter::~RubyCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (. and ::) to allow completion of
	fully qualified names.  We also include $ and @ since these specify
	variable types.

 ******************************************************************************/

bool
RubyCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || c == '$' || c == '@' ||
				(includeNS && (c == '.' || c == ':'));
}
