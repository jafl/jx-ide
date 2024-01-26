/******************************************************************************
 ChooseProcessDialog.h

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_ChooseProcessDialog
#define _H_ChooseProcessDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXIntegerInput;
class ProcessText;

class ChooseProcessDialog : public JXModalDialogDirector
{
public:

	ChooseProcessDialog();

	~ChooseProcessDialog() override;

	bool	GetProcessID(JInteger* pid) const;
	void	SetProcessID(const JInteger value);

private:

// begin JXLayout

	ProcessText*    itsText;
	JXIntegerInput* itsProcessIDInput;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
