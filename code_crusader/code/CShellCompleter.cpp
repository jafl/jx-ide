/******************************************************************************
 CShellCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "CShellCompleter.h"
#include <jx-af/jcore/jAssert.h>

CShellCompleter* CShellCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	"break", "breaksw", "case", "continue", "default", "else", "end",
	"endif", "endsw", "foreach", "if", "then", "repeat", "switch", "while",

	"true", "false", "alias", "alloc", "bg", "bindkey", "builtins", "bye",
	"cd", "chdir", "complete", "dirs", "echo", "echotc", "eval", "exec",
	"exit", "fg", "filetest", "getspath", "getxvers", "glob", "goto",
	"hashstat", "history", "hup", "inlib", "jobs", "kill", "limit", "log",
	"login", "logout", "migrate", "newgrp", "nice", "nohup", "notify",
	"onintr", "popd", "printenv", "pushd", "rehash", "rootnode", "sched",
	"set", "setenv", "setpath", "setspath", "settc", "setty", "setxvers",
	"shift", "source", "stop", "suspend", "telltc", "time", "umask", "unalias",
	"uncomplete", "unhash", "universe", "unlimit", "unset", "unsetenv", "ver",
	"wait", "warp", "watchlog", "where", "which"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
CShellCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew CShellCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CShellCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CShellCompleter::CShellCompleter()
	:
	StringCompleter(kCShellLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
	UpdateWordList();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CShellCompleter::~CShellCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

bool
CShellCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_';
}
