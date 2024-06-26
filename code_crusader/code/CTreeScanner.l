%top {
/*
Copyright © 1998 by Dustin Laurence.
Copyright © 2021 by John Lindal.

This scanner reads a C/C++ file and creates Class instances.
*/

#include "Tree.h"
#include "CClass.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JStack.h>
#include <jx-af/jcore/JFAID.h>
#include <jx-af/jcore/jAssert.h>
}

%option namespace="TreeScanner::C" lexer="Scanner"
%option lex="CreateClasses" token-type="bool"
%option params="const JFAID_t fileID, Tree* tree, JPtrArray<Class>* classList"
%option unicode nodefault full freespace

%class {

public:

	~Scanner() override;

private:

	JInteger			itsCurlyBraceDepth;
	bool				itsTemplateFlag;
	JInteger			itsTemplateBracketDepth;
	Class::InheritType	itsInheritType;
	Class*				itsCurrentClass;

	JPtrArray<JString>*					itsUsingList;
	JStack<JInteger, JArray<JInteger>>	itsCurlyBraceStack;

	JString				itsNamespaceStr;
	JPtrArray<JString>*	itsNamespaceStack;

private:

	void ResetState()
	{
		itsTemplateFlag = false;
		itsCurrentClass = nullptr;
	}
}

%init {
	itsUsingList      = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsNamespaceStack = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
}

%x NAMESPACE_STATE CLASS_NAME_STATE INHERIT_STATE
%x TEMPLATE_PARAM_STATE C_COMMENT_STATE CPP_COMMENT_STATE STRING_STATE

WS     [[:space:]]+
MS     [[:space:]]*

CKEYWORD     (auto|break|case|const|continue|default|do|else|enum|extern|for|goto|if|inline|register|restrict|return|sizeof|static|struct|switch|typedef|union|volatile|while)
CPPKEYWORD   (alignas|aslignof|and|and_eq|asm|atomic_cancel|atomic_commit|atomic_noexcept|bitand|bitor|catch|compl|concept|constexpr|const_cast|decltype|delete|dynamic_cast|explicit|export|false|final|friend|module|mutable|new|noexcept|not|not_eq|operator|or|or_eq|overload|override|private|protected|public|reinterpret_cast|requires|static_assert|static_cast|this|thread_local|throw|true|try|typeid|typename|virtual|xor|xor_eq)
PPKEYWORD    (include|include_next|define|undef|if|ifdef|ifndef|else|elif|endif|line|pragma|error)
CDATATYPE    (bool|char|char16_t|char32_t|double|float|int|long|nullptr|short|signed|unsigned|void|wchar_t)

HEXQUAD      ([[:xdigit:]]{4})
UESCCODE     (\\[uU]{HEXQUAD}{HEXQUAD}?)
IDCAR        (_|\p{L}|{UESCCODE})
IDCDR        (_|\p{L}|\d|{UESCCODE})
ID           ({IDCAR}{IDCDR}*)
NSID         ({ID}(::{ID})*)
ATTR         ("[["[^]]+"]]")

%%

%{
	ResetState();

	itsCurlyBraceDepth      = 0;
	itsTemplateBracketDepth = 0;
	itsInheritType          = Class::kInheritPrivate;

	itsUsingList->CleanOut();
	itsCurlyBraceStack.Clear();

	itsNamespaceStr.Clear();
	itsNamespaceStack->CleanOut();
%}

template\b {
	itsTemplateFlag         = true;
	itsTemplateBracketDepth = 0;
	push_state(TEMPLATE_PARAM_STATE);
	}

namespace\b {
	itsNamespaceStr.Clear();
	start(NAMESPACE_STATE);
	}

friend{WS}class{WS}{NSID} {
	ResetState();	/* ignore friend declarations */
	}

class{WS}({ATTR}{WS})*{NSID}{MS}; {
	ResetState();	/* ignore forward declarations */
	}

class\b {
	start(CLASS_NAME_STATE);
	}

using{WS}{NSID}{MS}; {
	JString* s = jnew JString(text()+5, size()-6);
	s->TrimWhitespace();
	itsUsingList->Append(s);
	}

"{" {
	itsCurlyBraceDepth++;
	}

"}" {
	itsCurlyBraceDepth--;
	if (itsCurlyBraceDepth < 0)
	{
		return false;
	}
	else if (itsCurlyBraceDepth == 0 && !itsCurlyBraceStack.IsEmpty())
	{
		itsCurlyBraceDepth = itsCurlyBraceStack.Pop();
		itsNamespaceStack->DeleteItem(1);
	}
	}

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//" {
	push_state(CPP_COMMENT_STATE);
	}

(L|u8?|U)?\" {
	start(STRING_STATE);
	}

{CKEYWORD}\b   |
{CPPKEYWORD}\b |
{PPKEYWORD}\b  |
{CDATATYPE}\b  {
	ResetState();
	}

.|\n	/* ignore */

<*><<EOF>> {
	return ! classList->IsEmpty();
	}


	/* Extract namespace */


<NAMESPACE_STATE>{

{NSID} {
	itsNamespaceStr.Set(text(), size());
	}

"{" {
	if (!itsNamespaceStr.IsEmpty())
	{
		itsCurlyBraceStack.Push(itsCurlyBraceDepth);
		itsCurlyBraceDepth = 1;

		itsNamespaceStack->Prepend(itsNamespaceStr);
	}
	else
	{
		itsCurlyBraceDepth++;
	}
	ResetState();
	start(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	ResetState();
	start(INITIAL);
	}

}


	/* Extract class name and create CClass */


<CLASS_NAME_STATE>{

{ID}({WS}final)? {
	JString fullName(text(), size());
	if (fullName.EndsWith("final"))
	{
		JStringIterator iter(&fullName, JStringIterator::kStartAtEnd);
		iter.RemovePrev(5);
		fullName.TrimWhitespace();
	}
	for (auto* s : *itsNamespaceStack)
	{
		fullName.Prepend("::");
		fullName.Prepend(*s);
	}

	Class* c;
	if (!tree->FindClass(fullName, &c, false))
	{
		c = jnew CClass(fullName, Class::kClassType, fileID, tree);
		c->SetTemplate(itsTemplateFlag);
		classList->Append(c);
	}

	ResetState();
	itsCurrentClass = c;	// must be after ResetState();
	}

: {
	if (itsCurrentClass != nullptr)
	{
		itsInheritType = Class::kInheritPrivate;
		start(INHERIT_STATE);
	}
	else
	{
		ResetState();
		start(INITIAL);
	}
	}

"{" {
	if (itsCurrentClass != nullptr)
	{
		itsCurlyBraceStack.Push(itsCurlyBraceDepth);
		itsCurlyBraceDepth = 1;
		itsNamespaceStack->Prepend(itsCurrentClass->GetName());
	}
	else
	{
		itsCurlyBraceDepth++;
	}
	ResetState();
	start(INITIAL);
	}

{WS} |
{ATTR}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	ResetState();
	start(INITIAL);
	}

}


	/* Extract base classes */


<INHERIT_STATE>{

virtual		/* ignore */

public {
	itsInheritType = Class::kInheritPublic;
	}

protected {
	itsInheritType = Class::kInheritProtected;
	}

private {
	itsInheritType = Class::kInheritPrivate;
	}

("::")?{NSID}"<"? {
	JString parentName(text(), size()), ns;
	if (parentName.StartsWith("::"))
	{
		JStringIterator iter(&parentName);
		iter.SkipNext(2);
		iter.RemoveAllPrev();
	}
	else
	{
		bool first = true;
		for (const auto* ns1 : *itsNamespaceStack)
		{
			if (!first)
			{
				ns.Prepend("::");
			}
			ns.Prepend(*ns1);
			first = false;
		}
	}

	if (parentName.GetLastCharacter() == '<')
	{
		itsTemplateBracketDepth = 1;
		push_state(TEMPLATE_PARAM_STATE);

		parentName.Set(text(), size()-1);
	}

	JString s = parentName;
	s.Prepend("::");

	bool found = false;
	while (true)
	{
		for (const auto* c : *itsUsingList)
		{
			if (!c->EndsWith(s))
			{
				continue;
			}

			JString s1 = *c;
			JStringIterator iter(&s1, JStringIterator::kStartAtEnd);
			iter.RemovePrev(s.GetCharacterCount());
			iter.Invalidate();

			s1 += "::";
			s1 += parentName;

			parentName = s1;
			found      = true;
			ns.Clear();
			break;
		}

		JStringIterator iter(&s, JStringIterator::kStartAtEnd);
		if (!found && iter.Prev("::") && !iter.AtBeginning())
		{
			iter.RemoveAllNext();
		}
		else
		{
			break;
		}
	}

	if (parentName != itsCurrentClass->GetName() &&
		parentName != itsCurrentClass->GetFullName())
	{
		itsCurrentClass->AddParent(itsInheritType, parentName, ns);
	}
	}

"{" {
	itsCurlyBraceStack.Push(itsCurlyBraceDepth);
	itsCurlyBraceDepth = 1;
	itsNamespaceStack->Prepend(itsCurrentClass->GetName());
	ResetState();
	start(INITIAL);
	}

"," {
	itsInheritType = Class::kInheritPrivate;
	}

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	ResetState();
	start(INITIAL);
	}

}


	/* Ignore template parameters */


<TEMPLATE_PARAM_STATE>{

"<" {
	itsTemplateBracketDepth++;
	}

">" {
	itsTemplateBracketDepth--;
	if (itsTemplateBracketDepth <= 0)
	{
		pop_state();
	}
	}

[^<>]+	/* ignore */

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

[^\n]+			/* ignore everything inside comment */

\n {
	pop_state();
	}

}


	/* Match strings -- based on the string scanner from the flex manual */
	/* except I allow ANSI wide character strings, may God save my soul. */


<STRING_STATE>{

\" |
\n {
	start(INITIAL);
	}

\\(.|\n)? |
[^\\\n\"]+ 		/* ignore everything inside quotes */

}

%%

TreeScanner::C::Scanner::~Scanner()
{
	jdelete itsUsingList;
	jdelete itsNamespaceStack;
}
