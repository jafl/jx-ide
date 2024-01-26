/******************************************************************************
 AboutDialog.h

	Copyright (C) 2002 by Glenn W. Bach.

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
