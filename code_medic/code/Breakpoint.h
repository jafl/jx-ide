/******************************************************************************
 Breakpoint.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_Breakpoint
#define _H_Breakpoint

#include "Location.h"

class Link;

class Breakpoint : virtual public JBroadcaster
{
public:

	enum Action
	{
		// Do not change these values after they are assigned,
		// because they are stored in debug_config

		kKeepBreakpoint = 0,
		kRemoveBreakpoint,
		kDisableBreakpoint
	};

public:

	Breakpoint(const JIndex debuggerIndex, const Location& location,
				 const JString& fn, const JString& addr,
				 const bool enabled, const Action action,
				 const JString& condition, const JSize ignoreCount);

	Breakpoint(const JIndex debuggerIndex,
				 const JString& fileName, const JIndex lineIndex,
				 const JString& fn, const JString& addr,
				 const bool enabled, const Action action,
				 const JString& condition, const JSize ignoreCount);

	// search target
	Breakpoint(const JString& fileName, const JIndex lineIndex);
	Breakpoint(const JString& addr);

	~Breakpoint();

	JIndex			GetDebuggerIndex() const;
	const Location&	GetLocation() const;
	const JString&	GetFileName() const;
	const JFileID&	GetFileID() const;
	JIndex			GetLineNumber() const;
	const JString&	GetFunctionName() const;
	const JString&	GetAddress() const;
	bool			IsEnabled() const;
	Action			GetAction() const;
	bool			HasCondition() const;
	bool			GetCondition(JString* condition) const;
	JSize			GetIgnoreCount() const;

	void	DisplayStatus() const;
	void	ToggleEnabled();
	void	RemoveCondition();
	void	SetIgnoreCount(const JSize count);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex		itsDebuggerIndex;
	Location	itsLocation;
	JString		itsFnName;
	JString		itsAddr;
	bool		itsEnabledFlag;
	Action		itsAction;
	JString		itsCondition;
	JSize		itsIgnoreCount;

private:

	void	BreakpointX();
};


/******************************************************************************
 Breakpoint index

 *****************************************************************************/

inline JIndex
Breakpoint::GetDebuggerIndex()
	const
{
	return itsDebuggerIndex;
}

/******************************************************************************
 Location

 *****************************************************************************/

inline const Location&
Breakpoint::GetLocation()
	const
{
	return itsLocation;
}

/******************************************************************************
 File name

 *****************************************************************************/

inline const JString&
Breakpoint::GetFileName()
	const
{
	return itsLocation.GetFileName();
}

/******************************************************************************
 File ID

 *****************************************************************************/

inline const JFileID&
Breakpoint::GetFileID()
	const
{
	return itsLocation.GetFileID();
}

/******************************************************************************
 Line number

 *****************************************************************************/

inline JSize
Breakpoint::GetLineNumber()
	const
{
	return itsLocation.GetLineNumber();
}

/******************************************************************************
 Function name

 *****************************************************************************/

inline const JString&
Breakpoint::GetFunctionName()
	const
{
	return itsFnName;
}

/******************************************************************************
 Address (string)

 *****************************************************************************/

inline const JString&
Breakpoint::GetAddress()
	const
{
	return itsAddr;
}

/******************************************************************************
 Enabled

 *****************************************************************************/

inline bool
Breakpoint::IsEnabled()
	const
{
	return itsEnabledFlag;
}

/******************************************************************************
 Action when breakpoint is hit

 *****************************************************************************/

inline Breakpoint::Action
Breakpoint::GetAction()
	const
{
	return itsAction;
}

/******************************************************************************
 Condition for when breakpoint is triggered

 *****************************************************************************/

inline bool
Breakpoint::HasCondition()
	const
{
	return !itsCondition.IsEmpty();
}

inline bool
Breakpoint::GetCondition
	(
	JString* condition
	)
	const
{
	*condition = itsCondition;
	return HasCondition();
}

/******************************************************************************
 Number of times breakpoint will be ignored

 *****************************************************************************/

inline JSize
Breakpoint::GetIgnoreCount()
	const
{
	return itsIgnoreCount;
}

#endif
