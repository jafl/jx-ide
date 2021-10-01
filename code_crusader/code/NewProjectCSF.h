/******************************************************************************
 NewProjectCSF.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_NewProjectCSF
#define _H_NewProjectCSF

#include <jx-af/jx/JXChooseSaveFile.h>
#include <jx-af/jcore/JPrefObject.h>
#include "BuildManager.h"		// need defn of MakefileMethod

class NewProjectSaveFileDialog;

class NewProjectCSF : public JXChooseSaveFile
{
public:

	NewProjectCSF();

	virtual ~NewProjectCSF();

	bool						GetProjectTemplate(JString* fullName) const;
	BuildManager::MakefileMethod	GetMakefileMethod() const;

protected:

	virtual JXSaveFileDialog*
	CreateSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JString& fileFilter, const JString& origName,
						 const JString& prompt, const JString& message) override;

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	NewProjectSaveFileDialog*		itsSaveDialog;
	JString							itsProjectTemplate;
	BuildManager::MakefileMethod	itsMakefileMethod;
};


/******************************************************************************
 GetProjectTemplate

 ******************************************************************************/

inline bool
NewProjectCSF::GetProjectTemplate
	(
	JString* fullName
	)
	const
{
	*fullName = itsProjectTemplate;
	return !fullName->IsEmpty();
}

/******************************************************************************
 GetMakefileMethod

 ******************************************************************************/

inline BuildManager::MakefileMethod
NewProjectCSF::GetMakefileMethod()
	const
{
	return itsMakefileMethod;
}

#endif
