/******************************************************************************
 PrefsDialog.h

	Interface for the PrefsDialog class

	Copyright (C) 2002 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_PrefsDialog
#define _H_PrefsDialog

#include <jx-af/jx/JXDialogDirector.h>

class PrefsDialog : public JXDialogDirector
{
public:

	PrefsDialog()
	~PrefsDialog() override;

protected:

	virtual bool	OKToDeactivate();
};

#endif
