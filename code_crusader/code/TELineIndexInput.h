/******************************************************************************
 TELineIndexInput.h

	Interface for the TELineIndexInput class

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_TELineIndexInput
#define _H_TELineIndexInput

#include "TECaretInputBase.h"

class TELineIndexInput : public TECaretInputBase
{
public:

	TELineIndexInput(JXStaticText* label, JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~TELineIndexInput();

	void	SetTE(JXTEBase* te, TECaretInputBase* colInput);

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	virtual void	Act(JXTEBase* te, const JIndex value) override;
	virtual JIndex	GetValue(JXTEBase* te) const override;
	virtual JIndex	GetValue(const JTextEditor::CaretLocationChanged& info) const override;

private:

	TECaretInputBase*	itsColInput;

private:

	// not allowed

	TELineIndexInput(const TELineIndexInput& source);
	const TELineIndexInput& operator=(const TELineIndexInput& source);
};

#endif
