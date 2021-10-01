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

#define CtagsHTMLDef " --langdef=jhtml "
#define CtagsHTMLID \
	" \"--regex-jhtml=/<[^>]+(id|name)=([^ >\\\"']+)/\\\\2/i,id/ei\" " \
	" \"--regex-jhtml=/<[^>]+(id|name)=\\\"([^\\\"']+)\\\"/\\\\2/i,id/ei\" " \
	" \"--regex-jhtml=/<[^>]+(id|name)='([^\\\"']+)'/\\\\2/i,id/ei\" "

#define CtagsLexDef " --langdef=jlex "
#define CtagsLexState \
	" \"--regex-jlex=/^<([^<>]+)>\\\\{/\\\\1/s,state/ei\" "

#define CtagsMakeDef " --langdef=jmake "
#define CtagsMakeTarget \
	" \"--regex-jmake=/^ *([A-Z0-9_]+)[ \t]*:([^=]|$)/\\\\1/t,target/ei\" "
#define CtagsMakeVariable \
	" \"--regex-jmake=/^[ \t]*([A-Z0-9_]+)[ \t]*:?=/\\\\1/v,variable/ei\" "

#endif
