/******************************************************************************
 BreakpointManager.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_BreakpointManager
#define _H_BreakpointManager

#include "Breakpoint.h"

class GetBreakpointsCmd;

class BreakpointManager : virtual public JBroadcaster
{
public:

	BreakpointManager(Link* link, GetBreakpointsCmd* cmd);

	virtual	~BreakpointManager();

	const JPtrArray<Breakpoint>&	GetBreakpoints() const;
	const JPtrArray<Breakpoint>&	GetOtherpoints() const;

	bool	HasBreakpointAt(const Location& loc) const;
	bool	GetBreakpoints(const JString& fileName,
						   JPtrArray<Breakpoint>* list) const;
	bool	GetBreakpoints(const Location& loc,
						   JPtrArray<Breakpoint>* list) const;

	void	EnableAll();
	void	DisableAll();

	void	ReadSetup(std::istream& input, const JFileVersion vers);
	void	WriteSetup(std::ostream& output) const;

	// called by GetBreakpoints, Breakpoint

	void	SetUpdateWhenStop(const bool update);

	void	UpdateBreakpoints(const JPtrArray<Breakpoint>& bpList,
							  const JPtrArray<Breakpoint>& otherList);
	void	BreakpointFileNameResolved(Breakpoint* bp);
	void	BreakpointFileNameInvalid(Breakpoint* bp);

	static JListT::CompareResult
		CompareBreakpointLocations(Breakpoint* const & bp1,
								   Breakpoint* const & bp2);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	Link*					itsLink;
	GetBreakpointsCmd*		itsCmd;
	JPtrArray<Breakpoint>*	itsBPList;
	JPtrArray<Breakpoint>*	itsSavedBPList;		// nullptr unless waiting to restore
	bool					itsRestoreBreakpointsFlag;
	bool					itsUpdateWhenStopFlag;
	JPtrArray<Breakpoint>*	itsOtherList;		// display only

private:

	// not allowed

	BreakpointManager(const BreakpointManager&) = delete;
	BreakpointManager& operator=(const BreakpointManager&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kBreakpointsChanged;

	class BreakpointsChanged : public JBroadcaster::Message
		{
		public:

			BreakpointsChanged()
				:
				JBroadcaster::Message(kBreakpointsChanged)
				{ };
		};
};


/******************************************************************************
 SetUpdateWhenStop

 ******************************************************************************/

inline void
BreakpointManager::SetUpdateWhenStop
	(
	const bool update
	)
{
	itsUpdateWhenStopFlag = update;
}

/******************************************************************************
 GetBreakpoints

 *****************************************************************************/

inline const JPtrArray<Breakpoint>&
BreakpointManager::GetBreakpoints()
	const
{
	return *itsBPList;
}

/******************************************************************************
 GetOtherpoints

 *****************************************************************************/

inline const JPtrArray<Breakpoint>&
BreakpointManager::GetOtherpoints()
	const
{
	return *itsOtherList;
}

#endif
