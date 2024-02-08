/******************************************************************************
 EditSearchPathsDialog.h

	Interface for the EditSearchPathsDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditSearchPathsDialog
#define _H_EditSearchPathsDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JString;
class JXTextButton;
class PathTable;
class DirList;

class EditSearchPathsDialog : public JXModalDialogDirector, public JPrefObject
{
public:

	EditSearchPathsDialog(const DirList& dirList);

	~EditSearchPathsDialog() override;

	void	AddDirectories(const JPtrArray<JString>& list);
	void	GetPathList(DirList* pathList) const;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

// begin JXLayout

	PathTable* itsTable;

// end JXLayout

private:

	void	BuildWindow(const DirList& dirList);
};

#endif
