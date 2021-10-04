/******************************************************************************
 CmdLineInput.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CmdLineInput
#define _H_CmdLineInput

#include <jx-af/jx/JXInputField.h>

class ExecOutputDocument;

class CmdLineInput : public JXInputField
{
public:

	CmdLineInput(ExecOutputDocument* doc, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~CmdLineInput();

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

private:

	ExecOutputDocument*	itsDoc;

private:

	// not allowed

	CmdLineInput(const CmdLineInput& source);
	const CmdLineInput& operator=(const CmdLineInput& source);
};

#endif