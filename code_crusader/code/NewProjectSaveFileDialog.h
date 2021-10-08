/******************************************************************************
 NewProjectSaveFileDialog.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_NewProjectSaveFileDialog
#define _H_NewProjectSaveFileDialog

#include <jx-af/jx/JXSaveFileDialog.h>
#include "BuildManager.h"		// need defn of MakefileMethod

class JXRadioGroup;
class JXTextMenu;

class NewProjectSaveFileDialog : public JXSaveFileDialog
{
public:

	static NewProjectSaveFileDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JString& fileFilter, const JString& templateFile,
			   const BuildManager::MakefileMethod method,
			   const JString& origName, const JString& prompt,
			   const JString& message = JString::empty);

	~NewProjectSaveFileDialog() override;

	bool							GetProjectTemplate(JString* fullName) const;
	BuildManager::MakefileMethod	GetMakefileMethod() const;

protected:

	NewProjectSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
							   const JString& fileFilter,
							   const BuildManager::MakefileMethod method);

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex							itsTemplateIndex;
	BuildManager::MakefileMethod	itsMakefileMethod;

// begin JXLayout

	JXRadioGroup* itsMethodRG;
	JXTextMenu*   itsTemplateMenu;

// end JXLayout

private:

	void	BuildWindow(const JString& origName, const JString& prompt,
						const JString& message = JString::empty);
	void	UpdateMakefileMethod();

	void	BuildTemplateMenu(const JString& templateFile);
	void	BuildTemplateMenuItems(const JString& path, const bool isUserPath,
								   JPtrArray<JString>* menuText,
								   const JString& templateFile,
								   JString** menuTextStr) const;

	bool	OKToReplaceFile(const JString& fullName,
							const JString& programName);
};

#endif
