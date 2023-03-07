/******************************************************************************
 ListChooseFileDialog.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_ListChooseFileDialog
#define _H_ListChooseFileDialog

#include <jx-af/jx/JXChooseFileDialog.h>

class JXRadioGroup;

class ListChooseFileDialog : public JXChooseFileDialog
{
public:

	static ListChooseFileDialog*
		Create(const JString& replaceListStr,
			   const JString& appendToListStr,
			   const SelectType selectType,
			   const JString& selectName = JString::empty,
			   const JString& fileFilter = JString::empty,
			   const JString& message = JString::empty);

	~ListChooseFileDialog() override;

	bool	ReplaceExisting() const;

protected:

	ListChooseFileDialog(const JString& fileFilter);

private:

// begin JXLayout

	JXRadioGroup* itsAppendReplaceRG;

// end JXLayout

private:

	void	BuildWindow(const JString& replaceListStr,
						const JString& appendToListStr,
						const SelectType selectType,
						const JString& selectName,
						const JString& message);
};

#endif
