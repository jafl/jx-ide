/******************************************************************************
 EditSearchPathsDialog.h

	Interface for the EditSearchPathsDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditSearchPathsDialog
#define _H_EditSearchPathsDialog

#include <jx-af/jx/JXDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JString;
class JXTextButton;
class PathTable;
class DirList;
class RelPathCSF;

class EditSearchPathsDialog : public JXDialogDirector, public JPrefObject
{
public:

	EditSearchPathsDialog(JXDirector* supervisor,
							const DirList& dirList,
							RelPathCSF* csf);

	~EditSearchPathsDialog() override;

	void	AddDirectories(const JPtrArray<JString>& list);
	void	GetPathList(DirList* pathList) const;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

	PathTable*	itsTable;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const DirList& dirList, RelPathCSF* csf);
};

#endif
