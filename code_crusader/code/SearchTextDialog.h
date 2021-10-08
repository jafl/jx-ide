/******************************************************************************
 SearchTextDialog.h

	Copyright © 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_SearchTextDialog
#define _H_SearchTextDialog

#include <jx-af/jx/JXSearchTextDialog.h>
#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JString.h>

class JProgressDisplay;
class JXFileListTable;
class JXPathInput;
class SearchPathHistoryMenu;
class SearchFilterHistoryMenu;
class ListCSF;

class SearchTextDialog : public JXSearchTextDialog, public JPrefObject
{
public:

	static SearchTextDialog*	Create();

	~SearchTextDialog() override;

	void	Activate() override;

	void	ShouldSearchFiles(const bool search);
	void	AddFileToSearch(const JString& fileName) const;
	void	ClearFileList();

protected:

	SearchTextDialog();

	void	UpdateDisplay() override;

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXFileListTable*	itsFileList;
	ListCSF*			itsCSF;

	mutable JString	itsFileSetName;
	bool			itsOnlyListFilesFlag;
	bool			itsListFilesWithoutMatchFlag;

// begin JXLayout

	JXTextCheckbox*          itsMultifileCB;
	JXTextMenu*              itsFileListMenu;
	JXTextCheckbox*          itsSearchDirCB;
	JXPathInput*             itsDirInput;
	SearchPathHistoryMenu*   itsDirHistory;
	JXInputField*            itsFileFilterInput;
	SearchFilterHistoryMenu* itsFileFilterHistory;
	JXTextCheckbox*          itsRecurseDirCB;
	JXTextButton*            itsChooseDirButton;
	JXTextCheckbox*          itsInvertFileFilterCB;
	JXInputField*            itsPathFilterInput;
	SearchFilterHistoryMenu* itsPathFilterHistory;

// end JXLayout

private:

	void	BuildWindow();

	void	UpdateFileListMenu();
	void	HandleFileListMenu(const JIndex item);

	void	SearchFiles() const;
	void	SearchFilesAndReplace();
	bool	BuildSearchFileList(JPtrArray<JString>* fileList,
								JPtrArray<JString>* nameList) const;
	bool	SearchDirectory(const JString& path, const JRegex* fileRegex,
							const JRegex* pathRegex,
							JPtrArray<JString>* fileList,
							JPtrArray<JString>* nameList,
							JProgressDisplay& pg) const;
	void	SaveFileForSearch(const JString& fullName,
							  JPtrArray<JString>* fileList,
							  JPtrArray<JString>* nameList) const;
	void	UpdateBasePath();
	bool	GetSearchDirectory(JString* path, JString* fileFilter,
							   JString *pathFilter) const;

	void	LoadFileSet();
	void	SaveFileSet() const;
	void	OpenSelectedFiles() const;
	void	AddSearchFiles();
};

#endif
