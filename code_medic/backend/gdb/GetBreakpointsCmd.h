/******************************************************************************
 GetBreakpointsCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetBreakpointsCmd
#define _H_GDBGetBreakpointsCmd

#include <GetBreakpointsCmd.h>
#include "Breakpoint.h"		// for Action
#include <jx-af/jcore/JStringPtrMap.h>

namespace gdb {

class GetBreakpointsCmd : public ::GetBreakpointsCmd
{
public:

	GetBreakpointsCmd();

	~GetBreakpointsCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	void	ParseBreakpoint(JStringPtrMap<JString>& map,
							JPtrArray<Breakpoint>* list);
	void	ParseOther(JStringPtrMap<JString>& map,
					   JPtrArray<Breakpoint>* list);

	bool	ParseCommon(JStringPtrMap<JString>& map, JIndex* bpIndex,
						Breakpoint::Action* action, bool* enabled,
						JSize* ignoreCount);
};

};

#endif
