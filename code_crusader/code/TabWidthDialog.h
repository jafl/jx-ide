/******************************************************************************
 TabWidthDialog.h

	Copyright © 2004 by John Lindal.

 ******************************************************************************/

#ifndef _H_TabWidthDialog
#define _H_TabWidthDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXIntegerInput;

class TabWidthDialog : public JXModalDialogDirector
{
public:

	TabWidthDialog(const JSize tabWidth);

	~TabWidthDialog() override;

	JSize	GetTabCharCount() const;

private:

// begin JXLayout

	JXIntegerInput* itsTabWidthInput;

// end JXLayout

private:

	void	BuildWindow(const JSize tabWidth);
};

#endif
