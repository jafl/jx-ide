/******************************************************************************
 CRMRuleListTable.h

	Interface for the CRMRuleListTable class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CRMRuleListTable
#define _H_CRMRuleListTable

#include <jx-af/jx/JXStringTable.h>
#include "PrefsManager.h"		// need definition of CRMRuleListInfo

class JXTextButton;
class CRMRuleTable;

class CRMRuleListTable : public JXStringTable
{
public:

	CRMRuleListTable(JArray<PrefsManager::CRMRuleListInfo>* crmList,
					   const JIndex initialSelection, const JIndex firstUnusedID,
					   CRMRuleTable* ruleTable,
					   JXTextButton* addRowButton, JXTextButton* removeRowButton,
					   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	~CRMRuleListTable() override;

	bool	GetCurrentCRMRuleSetName(JString* name) const;

	JArray<PrefsManager::CRMRuleListInfo>*
		GetCRMRuleLists(JIndex* firstNewID, JIndex* lastNewID) const;

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h) override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JArray<PrefsManager::CRMRuleListInfo>*	itsCRMList;

	mutable bool	itsOwnsCRMListFlag;		// true => delete contents of itsMacroList
	const JIndex	itsFirstNewID;			// first index to use for new sets
	JIndex			itsLastNewID;			// index of last new set created
	JIndex			itsCRMIndex;			// index of currently displayed macro set

	CRMRuleTable*	itsRuleTable;

	JXTextButton*	itsAddRowButton;
	JXTextButton*	itsRemoveRowButton;

private:

	void	AddRow();
	void	RemoveRow();
	void	SwitchDisplay();

	static JListT::CompareResult
		CompareNames(
			const PrefsManager::CRMRuleListInfo& i1,
			const PrefsManager::CRMRuleListInfo& i2);
};

#endif
