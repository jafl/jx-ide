/******************************************************************************
 VarCommand.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_VarCommand
#define _H_VarCommand

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class VarNode;

class VarCommand : public Command, virtual public JBroadcaster
{
public:

	VarCommand();

	virtual	~VarCommand();

	const JString&	GetData() const;

protected:

	void	SetData(const JString& data);

	virtual void	HandleFailure();

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

		public:

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
VarCommand::GetData()
	const
{
	return itsData;
}

/******************************************************************************
 SetData (protected)

 ******************************************************************************/

inline void
VarCommand::SetData
	(
	const JString& data
	)
{
	itsData = data;
}

#endif
