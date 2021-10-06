/******************************************************************************
 SubprojectConfigDialog.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_SubprojectConfigDialog
#define _H_SubprojectConfigDialog

#include <jx-af/jx/JXDialogDirector.h>

class JString;
class JXTextButton;
class JXTextCheckbox;
class ProjectDocument;
class ProjectFileInput;
class RelPathCSF;

class SubprojectConfigDialog : public JXDialogDirector
{
public:

	SubprojectConfigDialog(ProjectDocument* supervisor,
							 const bool includeInDepList,
							 const JString& subProjName,
							 const bool shouldBuild,
							 RelPathCSF* csf);

	virtual ~SubprojectConfigDialog();

	void	GetConfig(bool* includeInDepList,
					  JString* subProjName, bool* shouldBuild) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	RelPathCSF*	itsCSF;			// not owned

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
