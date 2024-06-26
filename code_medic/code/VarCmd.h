/******************************************************************************
 VarCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_VarCmd
#define _H_VarCmd

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class VarNode;

class VarCmd : public Command, virtual public JBroadcaster
{
public:

	VarCmd();

	~VarCmd() override;

	const JString&	GetData() const;

protected:

	void	SetData(const JString& data);

	void	HandleFailure() override;

private:

	JString	itsData;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kValueUpdated;
	static const JUtf8Byte* kValueFailed;

	class ValueMessage : public JBroadcaster::Message
	{
	public:

		ValueMessage(const JUtf8Byte* type, VarNode* root)
			:
			JBroadcaster::Message(type),
			itsRootNode(root)
		{ };

		VarNode*
		GetRootNode() const
		{
			return itsRootNode;
		}

	private:

		VarNode*	itsRootNode;
	};
};


/******************************************************************************
 GetData

 ******************************************************************************/

inline const JString&
VarCmd::GetData()
	const
{
	return itsData;
}

/******************************************************************************
 SetData (protected)

 ******************************************************************************/

inline void
VarCmd::SetData
	(
	const JString& data
	)
{
	itsData = data;
}

#endif
