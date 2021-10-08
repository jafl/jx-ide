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
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JString& fileFilter,
			   const bool allowSelectMultiple,
			   const JString& replaceListStr, const JString& appendToListStr,
			   const JString& origName, const JString& message);

	~ListChooseFileDialog() override;

	bool	ReplaceExisting() const;

protected:

	ListChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const bool allowSelectMultiple);

private:

// begin JXLayout

	JXRadioGroup* itsAppendReplaceRG;

// end JXLayout

private:

	void	BuildWindow(const JString& replaceListStr,
						const JString& appendToListStr,
						const JString& origName,
						const JString& message);
};

#endif
