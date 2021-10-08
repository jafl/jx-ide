/******************************************************************************
 ChooseProcessDialog.h

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_ChooseProcessDialog
#define _H_ChooseProcessDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXIntegerInput;
class ProcessText;

class ChooseProcessDialog : public JXDialogDirector
{
public:

	ChooseProcessDialog(JXDirector* supervisor,
						  const bool attachToSelection = true,
						  const bool stopProgram = false);

	~ChooseProcessDialog() override;

	void	SetProcessID(const JInteger value);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	ProcessText*	itsText;
	const bool		itsAttachToSelectionFlag;
	const bool		itsStopProgramFlag;

// begin JXLayout

	JXIntegerInput* itsProcessIDInput;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
