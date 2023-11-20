#ifndef _H_GDBVarGroupInfo
#define _H_GDBVarGroupInfo

/******************************************************************************
 VarGroupInfo.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JPtrArray-JString.h>

class VarNode;

struct GDBVarGroupInfo
{
	JString*			name;
	JPtrArray<VarNode>*	list;

	GDBVarGroupInfo(JString* n, JPtrArray<VarNode>* l)
		:
		name(n), list(l)
	{ };

	~GDBVarGroupInfo()
	{
		jdelete name;
		jdelete list;
	};

	const JString& GetName()
	{
		return (name == nullptr ? JString::empty : *name);
	};
};

#endif
