/******************************************************************************
 ExecHistoryMenu.h

	Interface for the ExecHistoryMenu class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_ExecHistoryMenu
#define _H_ExecHistoryMenu

#include <jx-af/jx/JXStringHistoryMenu.h>

class ExecHistoryMenu : public JXStringHistoryMenu
{
public:

	ExecHistoryMenu(const JSize historyLength,
					  const JString& title, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	ExecHistoryMenu(const JSize historyLength,
					  JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~ExecHistoryMenu();

private:

	void	ExecHistoryMenuX();
};

#endif
