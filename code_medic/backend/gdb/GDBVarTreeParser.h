#ifndef _H_GDBVarTreeParser
#define _H_GDBVarTreeParser

/******************************************************************************
 GDBVarTreeParser.h

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#include "GDBVarTreeScanner.h"

class VarNode;

class GDBVarTreeParser
{
public:

	GDBVarTreeParser();

	virtual	~GDBVarTreeParser();

	int		Parse(const JString& text);
	void	ReportRecoverableError();

	VarNode*	GetRootNode() const;

private:

	GDB::VarTree::Scanner*	itsScanner;
	VarNode*				itsCurrentNode;
	bool					itsIsPointerFlag;

	// compensate for gdb stopping on error

	JSize	itsGroupDepth;
	bool	itsGDBErrorFlag;

private:

	int		yyparse();

	void	AppendAsArrayElement(VarNode* node, JPtrArray<VarNode>* list) const;
	void	AppendAsArrayElement(const JString& groupLabel,
								 const JPtrArray<VarNode>& data,
								 JPtrArray<VarNode>* list) const;

	int yylex(YYSTYPE* lvalp);
	int yyerror(const char* message);

	// not allowed

	GDBVarTreeParser(const GDBVarTreeParser&) = delete;
	GDBVarTreeParser& operator=(const GDBVarTreeParser&) = delete;
};


/******************************************************************************
 GetRootNode

 *****************************************************************************/

inline VarNode*
GDBVarTreeParser::GetRootNode()
	const
{
	return itsCurrentNode;
}

#endif
