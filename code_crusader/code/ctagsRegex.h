/******************************************************************************
 ctagsRegex.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_ctagsRegex
#define _H_ctagsRegex

#define CtagsBisonDef " --langdef=jbison "
#define CtagsBisonNonterminalDef \
	" \"--regex-jbison=/^[ \t]*([[:alpha:]_.][[:alnum:]_.]*)[ \t\n]*:/" \
	"\\\\1/N,nontermdef/ei\" "
#define CtagsBisonNonterminalDecl \
	" \"--regex-jbison=/^[ \t]*%type([ \t\n]*<[^>]*>)?[ \t\n]*" \
	"([[:alpha:]_.][[:alnum:]_.]*)/\\\\2/n,nontermdecl/ei\" "
#define CtagsBisonTerminalDecl \
	" \"--regex-jbison=/^[ \t]*%(token|right|left|nonassoc)([ \t\n]*<[^>]*>)?[ \t\n]*" \
	"([[:alpha:]_.][[:alnum:]_.]*)/\\\\3/t,termdecl/ei\" "

#endif
