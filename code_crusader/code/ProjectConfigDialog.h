/******************************************************************************
 ProjectConfigDialog.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_ProjectConfigDialog
#define _H_ProjectConfigDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include "BuildManager.h"		// need defn on MakefileMethod

class JString;
class JXTextButton;
class JXRadioGroup;
class JXInputField;

class ProjectConfigDialog : public JXModalDialogDirector
{
public:

	ProjectConfigDialog(const BuildManager::MakefileMethod method,
						const JString& targetName,
						const JString& depListExpr,
						const JString& updateMakefileCmd,
						const JString& subProjectBuildCmd);

	~ProjectConfigDialog() override;

	void	GetConfig(BuildManager::MakefileMethod* method,
					  JString* targetName, JString* depListExpr,
					  JString* updateMakefileCmd, JString* subProjectBuildCmd) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	BuildManager::MakefileMethod	itsCurrentMethod;

// begin JXLayout

	JXRadioGroup* itsMethodRG;
	JXInputField* itsTargetName;
	JXInputField* itsDepListExpr;
	JXInputField* itsUpdateMakefileCmd;
	JXInputField* itsSubProjectBuildCmd;
	JXTextButton* itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const BuildManager::MakefileMethod method,
						const JString& targetName,
						const JString& depListExpr,
						const JString& updateMakefileCmd,
						const JString& subProjectBuildCmd);

	void	UpdateDisplay();
};

#endif
