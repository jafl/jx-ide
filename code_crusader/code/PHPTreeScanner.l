%top {
/*
Copyright © 2014 by John Lindal.

This scanner reads a PHP file and builds a PHPClass.
*/

#include "Tree.h"
#include "PHPClass.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JFAID.h>
#include <jx-af/jcore/jAssert.h>
}

%option namespace="TreeScanner::PHP" lexer="Scanner"
%option lex="CreateClass" token-type="bool"
%option params="const JFAID_t fileID, Tree* tree, Class** theClass"
%option unicode nodefault full freespace

%class {

public:

	~Scanner() override;

private:

	JString	itsPHPNamespace;
	bool	itsAbstractFlag;
	bool	itsFinalFlag;

	JString					itsUseName;
	JStringPtrMap<JString>*	itsUseMap;
}

%init {
	itsUseMap = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
}

%x NAMESPACE_STATE USE_STATE USE_AS_STATE CLASS_NAME_STATE INHERIT_STATE
%x C_COMMENT_STATE CPP_COMMENT_STATE

WS     [[:space:]]+

HEXQUAD      ([[:xdigit:]]{4})
UESCCODE     (\\[uU]{HEXQUAD})
IDCAR        (_|\p{L}|{UESCCODE})
IDCDR        (_|\p{L}|\d|{UESCCODE})
ID           ({IDCAR}{IDCDR}*)
NSID         ({ID}(\\{ID})*)

%%

%{
	itsAbstractFlag = false;
	itsFinalFlag    = false;
	*theClass       = nullptr;

	itsPHPNamespace.Clear();
	itsUseName.Clear();
	itsUseMap->CleanOut();
%}

namespace\b {
	start(NAMESPACE_STATE);
	}

use\b {
	start(USE_STATE);
	}

final\b {
	itsFinalFlag = true;
	}

abstract\b {
	itsAbstractFlag = true;
	}

class\b {
	start(CLASS_NAME_STATE);
	}

interface\b {
	start(CLASS_NAME_STATE);
	itsAbstractFlag = true;
	}

(extends|implements)\b {
	start(INHERIT_STATE);
	}

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	push_state(CPP_COMMENT_STATE);
	}

"{" {
	return *theClass != nullptr;
	}

.|\n	/* ignore */

<*><<EOF>> {
	return *theClass != nullptr;
	}


	/* Extract class namespace */


<NAMESPACE_STATE>{

{NSID} {
	itsPHPNamespace.Set(text(), size());
	start(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	return *theClass != nullptr;
	}

}


	/* Extract imported class */


<USE_STATE>{

as\b {
	start(USE_AS_STATE);
	}

{NSID} {
	JString* s = jnew JString(text(), size());
	itsUseMap->SetItem(*s, s, JPtrArrayT::kDelete);
	itsUseName = *s;
	}

; {
	start(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	return *theClass != nullptr;
	}

}


<USE_AS_STATE>{

{ID} {
	JString* s = jnew JString(text(), size());
	itsUseMap->SetItem(itsUseName, s, JPtrArrayT::kDelete);
	start(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	return *theClass != nullptr;
	}

}


	/* Extract class name and create PHPClass */


<CLASS_NAME_STATE>{

{ID} {
	JString fullName(text(), size());
	if (!itsPHPNamespace.IsEmpty())
	{
		fullName.Prepend("\\");
		fullName.Prepend(itsPHPNamespace);
	}

	if (!tree->FindClass(fullName, theClass, false))
	{
		*theClass = jnew PHPClass(fullName, Class::kClassType, fileID, tree, itsFinalFlag);
		(**theClass).SetAbstract(itsAbstractFlag);
	}

	start(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	return *theClass != nullptr;
	}

}


	/* Extract base classes */


<INHERIT_STATE>{

extends |
implements	/* ignore */

{NSID} {
	if (*theClass == nullptr)
	{
		return false;
	}

	JString parentName(text(), size());
	if (parentName.StartsWith("\\"))
	{
		parentName.Set(text()+1, size()-1);
	}
	else if (!parentName.Contains("\\"))
	{
		JString s = parentName;
		s.Prepend("\\");

		JStringPtrMapCursor<JString> cursor(itsUseMap);
		JString key;
		while (cursor.Next())
		{
			key              = cursor.GetKey();
			const JString* c = cursor.GetValue();
			if (parentName == *c || key.EndsWith(s))
			{
				parentName = key;
				break;
			}
		}
	}

	if (parentName != (**theClass).GetName() &&
		parentName != (**theClass).GetFullName())
	{
		(**theClass).AddParent(Class::kInheritPublic, parentName, JString::empty);
	}
	}

","		/* ignore */

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	return *theClass != nullptr;
	}

}


	/* Ignore C comments -- this is the fast comment scanner from the flex man page,
	   since Vern Paxon presumably can optimize such things far better than I can. */


<C_COMMENT_STATE>{

[^*\n]*        |
[^*\n]*\n      |
"*"+[^*/\n]*   |
"*"+[^*/\n]*\n		/* ignore everything inside comment */

"*"+"/" {
	pop_state();
	}

}


	/* Ignore C++ comments */


<CPP_COMMENT_STATE>{

[^\n]+				/* ignore everything inside comment */

\n {
	pop_state();
	}

}

%%

TreeScanner::PHP::Scanner::~Scanner()
{
	jdelete itsUseMap;
}
