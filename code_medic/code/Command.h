/******************************************************************************
 Command.h

	Copyright (C) 1997-2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_Command
#define _H_Command

#include <jx-af/jcore/JString.h>

class Command : virtual public JBroadcaster
{
public:

	enum State
	{
		kUnassigned,
		kPending,
		kExecuting
	};

public:

	Command(const JUtf8Byte* cmd,
			  const bool oneShot, const bool background);
	Command(const JString& cmd,
			  const bool oneShot, const bool background);

	virtual	~Command();

	// State information

	const JString&	GetCommand() const;
	JIndex			GetTransactionID() const;
	State			GetState() const;
	bool			IsOneShot() const;
	bool			IsBackground() const;
	bool			WillIgnoreResult() const;			// mainly for gdb-mi
	void			ShouldIgnoreResult(const bool ignore);

	// Execution

	bool	Send();

	// called by Link

	void			SetTransactionID(const JIndex id);
	virtual void	Starting();
	void			Accumulate(const JString& data);
	void			SaveResult(const JString& data);	// mainly for gdb-mi
	void			Finished(const bool success);

protected:

	void	SetCommand(const JUtf8Byte* cmd);
	void	SetCommand(const JString& cmd);

	virtual void	HandleSuccess(const JString& data) = 0;
	virtual void	HandleFailure();

	const JString&				GetLastResult() const;
	const JPtrArray<JString>&	GetResults() const;

private:

	JString				itsCommandString;
	JString				itsData;
	JPtrArray<JString>*	itsResultList;
	JIndex				itsID;
	State				itsState;
	bool				itsDeleteFlag;
	bool				itsBackgroundFlag;
	bool				itsIgnoreResultFlag;

private:

	// not allowed

	Command(const Command&) = delete;
	Command& operator=(const Command&) = delete;
};


/******************************************************************************
 GetCommand

 *****************************************************************************/

inline const JString&
Command::GetCommand()
	const
{
	return itsCommandString;
}

/******************************************************************************
 SetCommand (protected)

 *****************************************************************************/

inline void
Command::SetCommand
	(
	const JUtf8Byte* cmd
	)
{
	itsCommandString = cmd;
}

inline void
Command::SetCommand
	(
	const JString& cmd
	)
{
	itsCommandString = cmd;
}

/******************************************************************************
 GetTransactionID

 *****************************************************************************/

inline JIndex
Command::GetTransactionID()
	const
{
	return itsID;
}

/******************************************************************************
 GetState

 *****************************************************************************/

inline Command::State
Command::GetState()
	const
{
	return itsState;
}

/******************************************************************************
 IsOneShot

 *****************************************************************************/

inline bool
Command::IsOneShot()
	const
{
	return itsDeleteFlag;
}

/******************************************************************************
 IsBackground

 *****************************************************************************/

inline bool
Command::IsBackground()
	const
{
	return itsBackgroundFlag;
}

/******************************************************************************
 Ignore result

 *****************************************************************************/

inline bool
Command::WillIgnoreResult()
	const
{
	return itsIgnoreResultFlag;
}

inline void
Command::ShouldIgnoreResult
	(
	const bool ignore
	)
{
	itsIgnoreResultFlag = ignore;
}

/******************************************************************************
 Accumulate

 *****************************************************************************/

inline void
Command::Accumulate
	(
	const JString& data
	)
{
	itsData += data;
}

#endif
