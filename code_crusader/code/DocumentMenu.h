/******************************************************************************
 DocumentMenu.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_DocumentMenu
#define _H_DocumentMenu

#include <jx-af/jx/JXDocumentMenu.h>

class DocumentMenu : public JXDocumentMenu
{
public:

	DocumentMenu(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	DocumentMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~DocumentMenu() override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
};

#endif
