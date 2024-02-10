/******************************************************************************
 DiffFileDialog.h

	Interface for the DiffFileDialog class

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_DiffFileDialog
#define _H_DiffFileDialog

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXTextButton;
class JXCheckbox;
class JXTextCheckbox;
class JXTextMenu;
class JXInputField;
class JXFileInput;
class JXTabGroup;
class DiffStyleMenu;
class SVNFileInput;

class DiffFileDialog : public JXWindowDirector, public JPrefObject
{
public:

	DiffFileDialog(JXDirector* supervisor);

	~DiffFileDialog() override;

	void	SetFile1(const JString& fullName);
	void	SetFile2(const JString& fullName);
	void	SetCVSFile(const JString& fullName);
	void	SetSVNFile(const JString& fullName);
	void	SetGitFile(const JString& fullName);

	void	ViewDiffs(const JString& fullName1, const JString& fullName2,
					  const bool silent = false);
	void	ViewVCSDiffs(const JString& fullName,
						 const bool silent = false);
	void	ViewCVSDiffs(const JString& fullName,
						 const JString& rev1, const JString& rev2,
						 const bool silent = false);
	void	ViewSVNDiffs(const JString& fullName,
						 const JString& rev1, const JString& rev2,
						 const bool silent = false);
	void	ViewGitDiffs(const JString& fullName,
						 const JString& rev1, const JString& rev2,
						 const bool silent = false);

	const JString&	GetSmartDiffItemText(const bool onDisk,
										 const JString& fullName,
										 bool* enable) const;
	void			ViewDiffs(const bool onDisk,
							  const JString& fullName,
							  const bool silent = false);

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum
	{
		kPlainDiffTabIndex = 1,
		kCVSDiffTabIndex,
		kSVNDiffTabIndex,
		kGitDiffTabIndex,

		kTabCount = 4
	};

private:

	JIndex			itsTabIndex;
	DiffStyleMenu*	itsStyleMenu[ kTabCount ][2];
	JIndex			itsCVSRev1Cmd;
	JIndex			itsCVSRev2Cmd;
	JIndex			itsSVNRev1Cmd;
	JIndex			itsSVNRev2Cmd;
	JIndex			itsGitRev1Cmd;
	JIndex			itsGitRev2Cmd;

// begin JXLayout

	JXTabGroup*     itsTabGroup;
	SVNFileInput*   itsSVNFileInput;
	JXTextButton*   itsGitChooseButton;
	JXTextButton*   itsSVNChooseButton;
	JXTextButton*   itsCVSChooseButton;
	JXTextButton*   itsPlainChoose1Button;
	JXTextCheckbox* itsGitSummaryCB;
	JXTextCheckbox* itsSVNSummaryCB;
	JXTextCheckbox* itsCVSSummaryCB;
	DiffStyleMenu*  itsPlainOnly1StyleMenu;
	JXTextMenu*     itsGitRev1Menu;
	JXTextMenu*     itsSVNRev1Menu;
	JXTextMenu*     itsCVSRev1Menu;
	JXTextMenu*     itsGitRev2Menu;
	JXTextMenu*     itsSVNRev2Menu;
	JXTextMenu*     itsCVSRev2Menu;
	JXTextButton*   itsPlainChoose2Button;
	DiffStyleMenu*  itsPlainOnly2StyleMenu;
	DiffStyleMenu*  itsGitOnly1StyleMenu;
	DiffStyleMenu*  itsSVNOnly1StyleMenu;
	DiffStyleMenu*  itsCVSOnly1StyleMenu;
	DiffStyleMenu*  itsGitOnly2StyleMenu;
	DiffStyleMenu*  itsSVNOnly2StyleMenu;
	DiffStyleMenu*  itsCVSOnly2StyleMenu;
	JXTextCheckbox* itsIgnoreSpaceChangeCB;
	JXTextCheckbox* itsIgnoreBlankLinesCB;
	DiffStyleMenu*  itsCommonStyleMenu;
	JXTextCheckbox* itsStayOpenCB;
	JXTextButton*   itsCloseButton;
	JXTextButton*   itsHelpButton;
	JXTextButton*   itsViewButton;
	JXFileInput*    itsPlainFile1Input;
	JXFileInput*    itsPlainFile2Input;
	JXFileInput*    itsCVSFileInput;
	JXInputField*   itsCVSRev1Input;
	JXInputField*   itsCVSRev2Input;
	JXInputField*   itsSVNRev1Input;
	JXInputField*   itsSVNRev2Input;
	JXFileInput*    itsGitFileInput;
	JXInputField*   itsGitRev1Input;
	JXInputField*   itsGitRev2Input;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateDisplay();
	bool	ViewDiffs();

	bool	CheckFile(JXFileInput* widget) const;
	void	ChooseFile(JXFileInput* widget);
	void	ChoosePath(JXFileInput* widget);

	JString		BuildDiffCmd();
	void		DiffDirectory(const JString& fullName, const JString& diffCmd,
							  JXCheckbox* summaryCB, const JString& summaryArgs);

	bool	CheckVCSFileOrPath(JXFileInput* widget, const bool reportError,
							   JString* fullName) const;
	void	UpdateVCSRevMenu(JXTextMenu* menu, const JIndex cmd);

	void	ViewCVSDiffs(const JString& fullName, const bool silent);
	void	HandleCVSRevMenu(JXTextMenu* menu, const JIndex index,
							 JIndex* cmd, JXInputField* input);
	bool	BuildCVSDiffCmd(const JString& fullName,
							const JIndex rev1Cmd, const JString* rev1,
							const JIndex rev2Cmd, const JString* rev2,
							JString* getCmd, JString* diffCmd,
							JString* name1, JString* name2,
							const bool silent);

	bool	GetCurrentCVSRevision(const JString& fullName, JString* rev);
	bool	GetPreviousCVSRevision(const JString& fullName, JString* rev);

	void	ViewSVNDiffs(const JString& fullName, const bool silent);
	bool	CheckSVNFileOrPath(JXFileInput* widget, const bool reportError,
							   JString* fullName) const;
	void	HandleSVNRevMenu(JXTextMenu* menu, const JIndex index,
							 JIndex* cmd, JXInputField* input);
	bool	BuildSVNDiffCmd(const JString& fullName,
							const JIndex rev1Cmd, const JString* rev1,
							const JIndex rev2Cmd, const JString* rev2,
							JString* getCmd, JString* diffCmd,
							JString* name1, JString* name2,
							const bool silent,
							const bool forDirectory = false);
	bool	BuildSVNRepositoryPath(JString* fullName, const JIndex cmd,
								   const JString& rev, JString* name,
								   const bool silent);

	void	ViewGitDiffs(const JString& fullName, const bool silent);
	void	HandleGitRevMenu(JXTextMenu* menu, const JIndex index,
							 JIndex* cmd, JXInputField* input);
	bool	BuildGitDiffCmd(const JString& fullName,
							const JIndex rev1Cmd, const JString* rev1,
							const JIndex rev2Cmd, const JString* rev2,
							JString* get1Cmd, JString* get2Cmd, JString* diffCmd,
							JString* name1, JString* name2,
							const bool silent);
	bool	BuildGitDiffDirectoryCmd(const JString& path,
									 const JIndex rev1Cmd, const JString* rev1,
									 const JIndex rev2Cmd, const JString* rev2,
									 JString* diffCmd);
	bool	GetCurrentGitRevision(const JString& fullName, JString* rev);
	bool	GetPreviousGitRevision(const JString& fullName, JString* rev);
	bool	GetLatestGitRevisions(const JString& fullName,
								  JString* rev1, JString* rev2);
	bool	GetBestCommonGitAncestor(const JString& path,
									 JString* rev1, const JUtf8Byte* rev2);

	JString	GetSmartDiffInfo(const JString& origFileName,
							 bool* isSafetySave, bool* isBackup) const;

	void	UpdateBasePath();
};

#endif
