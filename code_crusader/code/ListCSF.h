/******************************************************************************
 ListCSF.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_ListChooseSaveFile
#define _H_ListChooseSaveFile

#include <jx-af/jx/JXChooseSaveFile.h>

class ListChooseFileDialog;

class ListCSF : public JXChooseSaveFile
{
public:

	ListCSF(const JString& replaceListStr,
			  const JString& appendToListStr);

	~ListCSF();

	bool	ReplaceExisting() const;

protected:

	JXChooseFileDialog*
	CreateChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const bool allowSelectMultiple,
						   const JString& origName, const JString& message) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	ListChooseFileDialog*	itsChooseDialog;
	bool					itsReplaceExistingFlag;
	JString					itsReplaceListStr;
	JString					itsAppendToListStr;
};


/******************************************************************************
 ReplaceExisting

 ******************************************************************************/

inline bool
ListCSF::ReplaceExisting()
	const
{
	return itsReplaceExistingFlag;
}

#endif
