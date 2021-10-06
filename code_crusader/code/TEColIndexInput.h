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

	virtual ~TEColIndexInput();

protected:

	virtual void	Act(JXTEBase* te, const JIndex value);
	virtual JIndex	GetValue(JXTEBase* te) const;
	virtual JIndex	GetValue(const JTextEditor::CaretLocationChanged& info) const;

private:

	TELineIndexInput*	itsLineInput;	// not owned
};

#endif
