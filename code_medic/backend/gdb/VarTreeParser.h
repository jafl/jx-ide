#ifndef _H_GDBVarTreeParser
#define _H_GDBVarTreeParser

/******************************************************************************
 VarTreeParser.h

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#include "VarNode.h"
#include "gdb/VarGroupInfo.h"
#include <jx-af/jcore/JString.h>
#include "gdb/VarTreeParserY.hpp"

class VarNode;

namespace gdb {

namespace VarTree { class Scanner; };

class VarTreeParser
{
public:

	VarTreeParser();

	~VarTreeParser();

	int		Parse(const JString& text);
	void	ReportRecoverableError();

	::VarNode*	GetRootNode() const;

private:

	gdb::VarTree::Scanner*	itsScanner;
	::VarNode*				itsCurrentNode;
	bool					itsIsPointerFlag;

	// compensate for gdb stopping on error

	JSize	itsGroupDepth;
	bool	itsGDBErrorFlag;

private:

	int		yyparse();

	void	AppendAsArrayElement(::VarNode* node, JPtrArray< ::VarNode>* list) const;
	void	AppendAsArrayElement(const JString& groupLabel,
								 const JPtrArray< ::VarNode>& data,
								 JPtrArray< ::VarNode>* list) const;

	int yylex(YYSTYPE* lvalp);
	int yyerror(const char* message);

	// not allowed

	VarTreeParser(const VarTreeParser&) = delete;
	VarTreeParser& operator=(const VarTreeParser&) = delete;
};


/******************************************************************************
 GetRootNode

 *****************************************************************************/

inline ::VarNode*
VarTreeParser::GetRootNode()
	const
{
	return itsCurrentNode;
}

};

#endif
