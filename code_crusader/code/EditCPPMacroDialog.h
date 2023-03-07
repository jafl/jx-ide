/******************************************************************************
 EditCPPMacroDialog.h

	Interface for the EditCPPMacroDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditCPPMacroDialog
#define _H_EditCPPMacroDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXTextButton;
class CPPMacroTable;
class CPreprocessor;

class EditCPPMacroDialog : public JXModalDialogDirector, public JPrefObject
{
public:

	EditCPPMacroDialog(const CPreprocessor& cpp);

	~EditCPPMacroDialog() override;

	bool	UpdateMacros(CPreprocessor* cpp) const;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	bool	OKToDeactivate() override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CPPMacroTable*	itsTable;

// begin JXLayout

	JXTextButton* itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const CPreprocessor& cpp);
};

#endif
