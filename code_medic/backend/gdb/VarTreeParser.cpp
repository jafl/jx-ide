/******************************************************************************
 VarTreeParser.cpp

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#include "gdb/VarTreeParser.h"
#include "gdb/VarTreeScanner.h"
#include "VarNode.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

gdb::VarTreeParser::VarTreeParser()
	:
	itsCurrentNode(nullptr),
	itsIsPointerFlag(false)
{
	itsScanner = jnew gdb::VarTree::Scanner;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

gdb::VarTreeParser::~VarTreeParser()
{
	jdelete itsScanner;
	jdelete itsCurrentNode;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

int
gdb::VarTreeParser::Parse
	(
	const JString& text
	)
{
	itsScanner->in(text.GetBytes());
	return yyparse();
}

/******************************************************************************
 yylex (private)

	Compensate for gdb stopping on error.

 *****************************************************************************/

int
gdb::VarTreeParser::yylex
	(
	YYSTYPE* lvalp
	)
{
	int token = itsScanner->NextToken(lvalp);
	if (token == P_GROUP_OPEN)
	{
		itsGroupDepth++;
	}
	else if (token == P_GROUP_CLOSE && itsGroupDepth > 0)
	{
		itsGroupDepth--;
	}
	else if (token == P_EOF && itsGroupDepth > 0)
	{
		itsGDBErrorFlag = true;

		token          = P_GROUP_CLOSE;
		lvalp->pString = jnew JString("}");

		itsGroupDepth--;
	}

	return token;
}

/******************************************************************************
 yyerror (private)

 *****************************************************************************/

int
gdb::VarTreeParser::yyerror
	(
	const char* message
	)
{
	std::cerr << "yyerror() called: " << message << std::endl;

	std::ostringstream log;
	log << "yyerror() called: " << message;
	Link::Log(log);

	return 0;
}

/******************************************************************************
 AppendAsArrayElement (private)

 *****************************************************************************/

void
gdb::VarTreeParser::AppendAsArrayElement
	(
	::VarNode*				node,
	JPtrArray< ::VarNode>*	list
	)
	const
{
	const JString name = "[" + JString(list->GetItemCount()) + "]";
	node->SetName(name);
	list->Append(node);
}

/******************************************************************************
 AppendAsArrayElement (private)

 *****************************************************************************/

void
gdb::VarTreeParser::AppendAsArrayElement
	(
	const JString&					groupLabel,
	const JPtrArray< ::VarNode>&	data,
	JPtrArray< ::VarNode>*			list
	)
	const
{
	const JString name = "[" + JString(list->GetItemCount()) + "]";

	::VarNode* node = GetLink()->CreateVarNode(nullptr, name, JString::empty, groupLabel);
	list->Append(node);

	const JSize count = data.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		node->Append(data.GetItem(i));
	}
}

/******************************************************************************
 ReportRecoverableError

 *****************************************************************************/

void
gdb::VarTreeParser::ReportRecoverableError()
{
	if (itsGDBErrorFlag)
	{
		::VarNode* child;
		itsCurrentNode->GetLastChild(&child);
		child->SetValid(false);

		child = GetLink()->CreateVarNode(itsCurrentNode,
					JGetString("ErrorNodeName::GDBVarTreeParser"), JString::empty,
					JGetString("ErrorNodeValue::GDBVarTreeParser"));
		child->SetValid(false);

		itsGDBErrorFlag = false;
	}
}
