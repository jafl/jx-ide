/******************************************************************************
 TEColIndexInput.h

	Interface for the TEColIndexInput class

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_TEColIndexInput
#define _H_TEColIndexInput

#include "TECaretInputBase.h"

class TELineIndexInput;

class TEColIndexInput : public TECaretInputBase
{
public:

	TEColIndexInput(TELineIndexInput* lineInput,
					  JXStaticText* label, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	~TEColIndexInput() override;

	using JXIntegerInput::GetValue;

protected:

	void	Act(JXTEBase* te, const JIndex value) override;
	JIndex	GetValue(JXTEBase* te) const override;
	JIndex	GetValue(const JTextEditor::CaretLocationChanged& info) const override;

private:

	TELineIndexInput*	itsLineInput;	// not owned
};

#endif
