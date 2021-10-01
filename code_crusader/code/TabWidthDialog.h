/******************************************************************************
 TabWidthDialog.h

	Copyright © 2004 by John Lindal.

 ******************************************************************************/

#ifndef _H_TabWidthDialog
#define _H_TabWidthDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXIntegerInput;

class TabWidthDialog : public JXDialogDirector
{
public:

	TabWidthDialog(JXWindowDirector* supervisor, const JSize tabWidth);

	virtual ~TabWidthDialog();

	JSize	GetTabCharCount() const;

private:

// begin JXLayout

	JXIntegerInput* itsTabWidthInput;

// end JXLayout

private:

	void	BuildWindow(const JSize tabWidth);

	// not allowed

	TabWidthDialog(const TabWidthDialog& source);
	const TabWidthDialog& operator=(const TabWidthDialog& source);
};

#endif
