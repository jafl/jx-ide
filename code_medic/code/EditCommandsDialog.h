/******************************************************************************
 EditCommandsDialog.h

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_EditCommandsDialog
#define _H_EditCommandsDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JPtrArray-JString.h>

class JXTextButton;
class EditCommandsTable;

class EditCommandsDialog : public JXModalDialogDirector, public JPrefObject
{
public:

	EditCommandsDialog();
	~EditCommandsDialog() override;

	JSize						GetStringCount() const;
	const JPtrArray<JString>&	GetStringList() const;

	const JString&	GetString(const JIndex index) const;
	void			SetString(const JIndex index, const JString& str);
	void			AppendString(const JString& str);
	void			DeleteString(const JIndex index);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	bool	OKToDeactivate() override;

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

	JPtrArray<JString>*	itsCommands;
	EditCommandsTable*	itsWidget;

// begin JXLayout

	JXTextButton* itsNewButton;
	JXTextButton* itsRemoveButton;

// end JXLayout

private:

	void	BuildWindow();
};


/******************************************************************************
 GetStringCount

 ******************************************************************************/

inline JSize
EditCommandsDialog::GetStringCount()
	const
{
	return itsCommands->GetElementCount();
}

/******************************************************************************
 GetString

 ******************************************************************************/

inline const JString&
EditCommandsDialog::GetString
	(
	const JIndex index
	)
	const
{
	return *(itsCommands->GetElement(index));
}

/******************************************************************************
 GetStringList

 ******************************************************************************/

inline const JPtrArray<JString>&
EditCommandsDialog::GetStringList()
	const
{
	return *itsCommands;
}

#endif
