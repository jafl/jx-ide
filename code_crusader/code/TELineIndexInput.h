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

	~TELineIndexInput() override;

	void	SetTE(JXTEBase* te, TECaretInputBase* colInput);

	void	HandleKeyPress(const JUtf8Character& c,
						   const int keySym, const JXKeyModifiers& modifiers) override;

	using JXIntegerInput::GetValue;

protected:

	void	Act(JXTEBase* te, const JIndex value) override;
	JIndex	GetValue(JXTEBase* te) const override;
	JIndex	GetValue(const JTextEditor::CaretLocationChanged& info) const override;

private:

	TECaretInputBase*	itsColInput;
};

#endif
