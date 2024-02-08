/******************************************************************************
 SaveNewProjectDialog.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_NewProjectSaveFileDialog
#define _H_NewProjectSaveFileDialog

#include <jx-af/jx/JXSaveFileDialog.h>
#include <jx-af/jcore/JPrefObject.h>
#include "BuildManager.h"		// for MakefileMethod

class JXRadioGroup;
class JXTextMenu;

class SaveNewProjectDialog : public JXSaveFileDialog, public JPrefObject
{
public:

	static SaveNewProjectDialog*
		Create(const JString& prompt,
			   const JString& startName = JString::empty,
			   const JString& fileFilter = JString::empty,
			   const JString& message = JString::empty);

	~SaveNewProjectDialog() override;

	bool							GetProjectTemplate(JString* fullName) const;
	BuildManager::MakefileMethod	GetMakefileMethod() const;

protected:

	SaveNewProjectDialog(const JString& fileFilter);

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex	itsTemplateIndex;

	JString							itsProjectTemplate;
	BuildManager::MakefileMethod	itsMakefileMethod;

// begin JXLayout

	JXTextMenu*   itsTemplateMenu;
	JXRadioGroup* itsMethodRG;

// end JXLayout

private:

	void	BuildWindow(const JString& startName,
						const JString& prompt,
						const JString& message);
	void	UpdateMakefileMethod();

	void	BuildTemplateMenu();
	void	BuildTemplateMenuItems(const JString& path, const bool isUserPath,
								   JPtrArray<JString>* menuText,
								   JString** menuTextStr) const;

	bool	OKToReplaceFile(const JString& fullName,
							const JString& programName);
};

#endif
