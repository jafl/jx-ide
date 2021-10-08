/******************************************************************************
 AboutDialog.h

	Interface for the AboutDialog class

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_AboutDialog
#define _H_AboutDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXTextButton;

class AboutDialog : public JXDialogDirector
{
public:

	AboutDialog(JXDirector* supervisor, const JString& prevVersStr);

	~AboutDialog() override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsIsUpgradeFlag;

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXTextButton* itsCreditsButton;

// end JXLayout

private:

	void	BuildWindow(const JString& prevVersStr);
};

#endif
