/******************************************************************************
 PrefsDialog.h

	Copyright (C) 2002 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_PrefsDialog
#define _H_PrefsDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JString.h>

class JXInputField;
class JXTextButton;

class PrefsDialog : public JXModalDialogDirector
{
public:

	PrefsDialog(const JString& header,
				const JString& source,
				const JString& constructor,
				const JString& destructor,
				const JString& function);

	~PrefsDialog() override;

	void	GetValues(JString* header, JString* source,
					  JString* constructor, JString* destructor,
					  JString* function);

protected:

	bool	OKToDeactivate() override;

private:

// begin JXLayout

	JXInputField* itsHeaderInput;
	JXInputField* itsSourceInput;
	JXInputField* itsConstructorInput;
	JXInputField* itsDestructorInput;
	JXInputField* itsFunctionInput;
	JXTextButton* itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const JString& header,
						const JString& source,
						const JString& constructor,
						const JString& destructor,
						const JString& function);
};

#endif
