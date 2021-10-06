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

	DocumentMenu(const JString& title, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	DocumentMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~DocumentMenu();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	// not allowed

	DocumentMenu(const DocumentMenu& source);
	const DocumentMenu& operator=(const DocumentMenu& source);
};

#endif
