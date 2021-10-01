/******************************************************************************
 AboutDialog.h

	Interface for the AboutDialog class

	Copyright © 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_AboutDialog
#define _H_AboutDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXTextButton;
class AboutDialogIconTask;

class AboutDialog : public JXDialogDirector
{
public:

	AboutDialog(JXDirector* supervisor, const JString& prevVersStr);

	virtual ~AboutDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool				itsIsUpgradeFlag;
	AboutDialogIconTask*	itsAnimTask;

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXTextButton* itsCreditsButton;

// end JXLayout

private:

	void	BuildWindow(const JString& prevVersStr);

	// not allowed

	AboutDialog(const AboutDialog& source);
	const AboutDialog& operator=(const AboutDialog& source);
};

#endif
