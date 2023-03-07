/******************************************************************************
 SubprojectConfigDialog.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_SubprojectConfigDialog
#define _H_SubprojectConfigDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXTextButton;
class JXTextCheckbox;
class ProjectDocument;
class ProjectFileInput;

class SubprojectConfigDialog : public JXModalDialogDirector
{
public:

	SubprojectConfigDialog(ProjectDocument* supervisor,
						   const bool includeInDepList,
						   const JString& subProjName,
						   const bool shouldBuild);

	~SubprojectConfigDialog() override;

	void	GetConfig(bool* includeInDepList,
					  JString* subProjName, bool* shouldBuild) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextCheckbox*   itsShouldBuildCB;
	ProjectFileInput* itsSubProjName;
	JXTextButton*     itsChooseFileButton;
	JXTextCheckbox*   itsIncludeInDepListCB;

// end JXLayout

private:

	void	BuildWindow(ProjectDocument* supervisor,
						const bool includeInDepList,
						const JString& subProjName,
						const bool shouldBuild,
						const JString& basePath);
	void	UpdateDisplay();

	void	ChooseProjectFile();
};

#endif
