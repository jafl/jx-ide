/******************************************************************************
 EditCRMDialog.h

	Interface for the EditCRMDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditCRMDialog
#define _H_EditCRMDialog

#include <jx-af/jx/JXDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include "PrefsManager.h"		// need definition of CRMRuleListInfo

class JXTextButton;
class JXVertPartition;
class CRMRuleListTable;
class CRMRuleTable;

class EditCRMDialog : public JXDialogDirector, public JPrefObject
{
public:

	EditCRMDialog(JArray<PrefsManager::CRMRuleListInfo>* crmList,
					const JIndex initialSelection,
					const JIndex firstUnusedID);

	virtual ~EditCRMDialog();

	bool	GetCurrentCRMRuleSetName(JString* name) const;

	JArray<PrefsManager::CRMRuleListInfo>*
		GetCRMRuleLists(JIndex* firstNewID, JIndex* lastNewID) const;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CRMRuleListTable*	itsCRMTable;
	CRMRuleTable*		itsRuleTable;

// begin JXLayout

	JXVertPartition* itsPartition;
	JXTextButton*    itsHelpButton;

// end JXLayout

// begin ruleLayout


// end ruleLayout

// begin crmLayout


// end crmLayout

private:

	void	BuildWindow(JArray<PrefsManager::CRMRuleListInfo>* crmList,
						const JIndex initialSelection,
						const JIndex firstUnusedID);

	// not allowed

	EditCRMDialog(const EditCRMDialog& source);
	const EditCRMDialog& operator=(const EditCRMDialog& source);
};

#endif
