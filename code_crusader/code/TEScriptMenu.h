/******************************************************************************
 TEScriptMenu.h

	Copyright © 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_TEScriptMenu
#define _H_TEScriptMenu

#include <jx-af/jfs/JXFSDirMenu.h>

class TextEditor;

class TEScriptMenu : public JXFSDirMenu
{
public:

	TEScriptMenu(TextEditor* te, const JString& path,
				   const JString& title, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	TEScriptMenu(TextEditor* te, const JString& path,
				   JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~TEScriptMenu() override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	TextEditor*	itsTE;

private:

	void	TEScriptMenuX();
	bool	UpdateSelf();
	bool	HandleSelection(const JIndex index);

	static bool	ShowExecutables(const JDirEntry& entry);
};

#endif
