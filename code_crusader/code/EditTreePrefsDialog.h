/******************************************************************************
 EditTreePrefsDialog.h

	Interface for the EditTreePrefsDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditTreePrefsDialog
#define _H_EditTreePrefsDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXTextButton;
class JXTextCheckbox;
class JXRadioGroup;
class JXFontSizeMenu;

class EditTreePrefsDialog : public JXModalDialogDirector
{
public:

	EditTreePrefsDialog(const JSize fontSize,
						const bool autoMinMILinks, const bool drawMILinksOnTop,
						const bool raiseWhenSingleMatch);

	~EditTreePrefsDialog() override;

	void	UpdateSettings();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum
	{
		kDrawMILinksAbove = 1,
		kDrawMILinksBelow
	};

private:

// begin JXLayout

	JXFontSizeMenu* itsFontSizeMenu;
	JXTextCheckbox* itsRaiseSingleMatchCB;
	JXTextCheckbox* itsAutoMinMILinkCB;
	JXRadioGroup*   itsMILinkStyleRG;
	JXTextButton*   itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const JSize fontSize,
						const bool autoMinMILinks, const bool drawMILinksOnTop,
						const bool raiseWhenSingleMatch);
};

#endif
