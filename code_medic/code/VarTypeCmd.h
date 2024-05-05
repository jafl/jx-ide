/******************************************************************************
 VarTypeCmd.h

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#ifndef _H_VarTypeCmd
#define _H_VarTypeCmd

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class VarNode;

class VarTypeCmd : public Command, virtual public JBroadcaster
{
public:

	VarTypeCmd();

	~VarTypeCmd() override;

	const JString&	GetData() const;

protected:

	void	SetData(const JString& data);

	void	HandleFailure() override;

private:

	JString	itsData;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kTypeInfo;

	class TypeInfo : public JBroadcaster::Message
	{
	public:

		TypeInfo(const JString& type)
			:
			JBroadcaster::Message(kTypeInfo),
			itsType(type)
		{ };

		const JString&
		GetType() const
		{
			return itsType;
		}

	private:

		JString	itsType;
	};
};


/******************************************************************************
 GetData

 ******************************************************************************/

inline const JString&
VarTypeCmd::GetData()
	const
{
	return itsData;
}

/******************************************************************************
 SetData (protected)

 ******************************************************************************/

inline void
VarTypeCmd::SetData
	(
	const JString& data
	)
{
	itsData = data;
}

#endif
