/******************************************************************************
 EditTreePrefsDialog.h

	Interface for the EditTreePrefsDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditTreePrefsDialog
#define _H_EditTreePrefsDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXTextButton;
class JXTextCheckbox;
class JXRadioGroup;
class JXFontSizeMenu;

class EditTreePrefsDialog : public JXDialogDirector
{
public:

	EditTreePrefsDialog(const JSize fontSize, const bool showInheritedFns,
						  const bool autoMinMILinks, const bool drawMILinksOnTop,
						  const bool raiseWhenSingleMatch);

	virtual ~EditTreePrefsDialog();

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

	JXTextCheckbox* itsShowInheritedFnsCB;
	JXFontSizeMenu* itsFontSizeMenu;
	JXTextCheckbox* itsAutoMinMILinkCB;
	JXRadioGroup*   itsMILinkStyleRG;
	JXTextButton*   itsHelpButton;
	JXTextCheckbox* itsRaiseSingleMatchCB;

// end JXLayout

private:

	void	BuildWindow(const JSize fontSize, const bool showInheritedFns,
						const bool autoMinMILinks, const bool drawMILinksOnTop,
						const bool raiseWhenSingleMatch);
	void	UpdateSettings();
};

#endif
