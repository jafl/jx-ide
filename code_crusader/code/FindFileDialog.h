/******************************************************************************
 FindFileDialog.h

	Interface for the FindFileDialog class

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_FindFileDialog
#define _H_FindFileDialog

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXTextButton;
class JXTextCheckbox;
class JXInputField;
class JXStringHistoryMenu;

class FindFileDialog : public JXWindowDirector, public JPrefObject
{
public:

	FindFileDialog(JXDirector* supervisor);

	virtual ~FindFileDialog();

	virtual void	Activate() override;

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextButton*        itsCloseButton;
	JXTextButton*        itsFindButton;
	JXInputField*        itsFileName;
	JXStringHistoryMenu* itsFileHistoryMenu;
	JXTextCheckbox*      itsIgnoreCaseCB;
	JXTextCheckbox*      itsStayOpenCB;

// end JXLayout

private:

	void		BuildWindow();
	void		UpdateDisplay();
	bool	FindFile();

	// not allowed

	FindFileDialog(const FindFileDialog& source);
	const FindFileDialog& operator=(const FindFileDialog& source);
};

#endif
