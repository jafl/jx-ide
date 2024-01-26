/******************************************************************************
 SearchFilterHistoryMenu.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_SearchFilterHistoryMenu
#define _H_SearchFilterHistoryMenu

#include <jx-af/jx/JXHistoryMenuBase.h>

class SearchFilterHistoryMenu : public JXHistoryMenuBase
{
public:

	SearchFilterHistoryMenu(const JSize historyLength, JXContainer* enclosure,
							  const HSizingOption hSizing, const VSizingOption vSizing,
							  const JCoordinate x, const JCoordinate y,
							  const JCoordinate w, const JCoordinate h);

	SearchFilterHistoryMenu(const JSize historyLength,
							  JXMenu* owner, const JIndex itemIndex,
							  JXContainer* enclosure);

	~SearchFilterHistoryMenu() override;

	void	AddFilter(const JString& filter, const bool invert);

	const JString&	GetFilter(const Message& message, bool* invert) const;
	const JString&	GetFilter(const JIndex index, bool* invert) const;
};

#endif
