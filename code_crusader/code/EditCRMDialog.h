/******************************************************************************
 EditCRMDialog.h

	Interface for the EditCRMDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditCRMDialog
#define _H_EditCRMDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include "PrefsManager.h"		// for CRMRuleListInfo

class JXTextButton;
class JXVertPartition;
class CRMRuleListTable;
class CRMRuleTable;

class EditCRMDialog : public JXModalDialogDirector, public JPrefObject
{
public:

	EditCRMDialog(JArray<PrefsManager::CRMRuleListInfo>* crmList,
					const JIndex initialSelection,
					const JIndex firstUnusedID);

	~EditCRMDialog() override;

	bool	GetCurrentCRMRuleSetName(JString* name) const;

	JArray<PrefsManager::CRMRuleListInfo>*
		GetCRMRuleLists(JIndex* firstNewID, JIndex* lastNewID) const;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXVertPartition*  itsPartition;
	JXTextButton*     itsHelpButton;
	CRMRuleTable*     itsRuleTable;
	CRMRuleListTable* itsCRMTable;

// end JXLayout

private:

	void	BuildWindow(JArray<PrefsManager::CRMRuleListInfo>* crmList,
						const JIndex initialSelection,
						const JIndex firstUnusedID);
};

#endif
