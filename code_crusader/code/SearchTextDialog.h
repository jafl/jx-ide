/******************************************************************************
 SearchTextDialog.h

	Copyright © 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_SearchTextDialog
#define _H_SearchTextDialog

#include <jx-af/jx/JXSearchTextDialog.h>
#include <jx-af/jcore/JPrefObject.h>

class JProgressDisplay;
class JXTextMenu;
class JXFileListTable;
class JXPathInput;
class SearchPathHistoryMenu;
class SearchFilterHistoryMenu;

class SearchTextDialog : public JXSearchTextDialog, public JPrefObject
{
public:

	static SearchTextDialog*	Create();

	~SearchTextDialog() override;

	void	Activate() override;

protected:

	SearchTextDialog();

	void	UpdateDisplay() override;

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextCheckbox*          itsSearchDirCB;
	SearchPathHistoryMenu*   itsDirHistory;
	JXTextButton*            itsChooseDirButton;
	SearchFilterHistoryMenu* itsFileFilterHistory;
	JXTextCheckbox*          itsInvertFileFilterCB;
	JXTextCheckbox*          itsRecurseDirCB;
	SearchFilterHistoryMenu* itsPathFilterHistory;
	JXTextCheckbox*          itsOnlyListFilesCB;
	JXTextCheckbox*          itsInvertMatchingCB;
	JXPathInput*             itsDirInput;
	JXInputField*            itsFileFilterInput;
	JXInputField*            itsPathFilterInput;

// end JXLayout

private:

	void	BuildWindow();

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
};

#endif
