/******************************************************************************
 AboutDialog.h

	Interface for the AboutDialog class

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_AboutDialog
#define _H_AboutDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXTextButton;

class AboutDialog : public JXModalDialogDirector
{
public:

	AboutDialog(const JString& prevVersStr);

	~AboutDialog() override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsIsUpgradeFlag;

// begin JXLayout

	JXTextButton* itsCreditsButton;
	JXTextButton* itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const JString& prevVersStr);
};

#endif
