/******************************************************************************
 EditCPPMacroDialog.h

	Interface for the EditCPPMacroDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditCPPMacroDialog
#define _H_EditCPPMacroDialog

#include <jx-af/jx/JXDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXTextButton;
class CPPMacroTable;
class CPreprocessor;

class EditCPPMacroDialog : public JXDialogDirector, public JPrefObject
{
public:

	EditCPPMacroDialog(JXDirector* supervisor, const CPreprocessor& cpp);

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
