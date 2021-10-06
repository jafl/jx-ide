/******************************************************************************
 SearchPathHistoryMenu.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_SearchPathHistoryMenu
#define _H_SearchPathHistoryMenu

#include <jx-af/jx/JXHistoryMenuBase.h>

class SearchPathHistoryMenu : public JXHistoryMenuBase
{
public:

	SearchPathHistoryMenu(const JSize historyLength,
							const JString& title, JXContainer* enclosure,
							const HSizingOption hSizing, const VSizingOption vSizing,
							const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h);

	SearchPathHistoryMenu(const JSize historyLength,
							JXMenu* owner, const JIndex itemIndex,
							JXContainer* enclosure);

	virtual ~SearchPathHistoryMenu();

	void	AddPath(const JString& fullName, const bool recurse);

	const JString&	GetPath(const Message& message, bool* recurse) const;
	const JString&	GetPath(const JIndex index, bool* recurse) const;

protected:

	virtual void	UpdateMenu();		// must call inherited

private:

	void	SearchPathHistoryMenuX();
	void	RemoveInvalidPaths();
};

#endif
