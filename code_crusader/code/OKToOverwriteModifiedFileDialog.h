/******************************************************************************
 OKToOverwriteModifiedFileDialog.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_OKToOverwriteModifiedFileDialog
#define _H_OKToOverwriteModifiedFileDialog

#include <jx-af/jx/JXUNDialogBase.h>

class JXTextButton;

class OKToOverwriteModifiedFileDialog : public JXUNDialogBase
{
public:

	enum CloseAction
	{
		kSaveData,
		kCompareData,
		kDontClose
	};

public:

	OKToOverwriteModifiedFileDialog(const JString& fileName);

	~OKToOverwriteModifiedFileDialog() override;

	CloseAction	GetCloseAction() const;

private:

	CloseAction	itsCloseAction;

// begin JXLayout

	JXTextButton* itsCompareButton;

// end JXLayout

private:

	void	BuildWindow(const JString& message);
};


/******************************************************************************
 GetCloseAction

 ******************************************************************************/

inline OKToOverwriteModifiedFileDialog::CloseAction
OKToOverwriteModifiedFileDialog::GetCloseAction()
	const
{
	return Cancelled() ? kDontClose : itsCloseAction;
}

#endif
