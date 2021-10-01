/******************************************************************************
 ViewManPageDialog.h

	Interface for the ViewManPageDialog class

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_ViewManPageDialog
#define _H_ViewManPageDialog

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXTextButton;
class JXInputField;
class JXCharInput;
class JXTextCheckbox;
class JXStringHistoryMenu;

class ViewManPageDialog : public JXWindowDirector, public JPrefObject
{
public:

	ViewManPageDialog(JXDirector* supervisor);

	virtual ~ViewManPageDialog();

	virtual void	Activate() override;

	// called by ManPageDocument

	void	AddToHistory(const JString& pageName, const JString& pageIndex,
						 const bool apropos);

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextButton*        itsCloseButton;
	JXTextButton*        itsViewButton;
	JXInputField*        itsFnName;
	JXTextCheckbox*      itsAproposCheckbox;
	JXTextButton*        itsHelpButton;
	JXStringHistoryMenu* itsFnHistoryMenu;
	JXTextCheckbox*      itsStayOpenCB;
	JXInputField*        itsManIndex;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateDisplay();
	void	ViewManPage();
	void	SetFunction(const JString& historyStr);

	// not allowed

	ViewManPageDialog(const ViewManPageDialog& source);
	const ViewManPageDialog& operator=(const ViewManPageDialog& source);
};

#endif
