/******************************************************************************
 CRMRuleTable.h

	Interface for the CRMRuleTable class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CRMRuleTable
#define _H_CRMRuleTable

#include <jx-af/jx/JXStringTable.h>
#include <jx-af/jcore/JStyledText.h>		// need definition of CRMRuleList

class JXTextButton;
class JXColHeaderWidget;
class EditCRMDialog;
class ListCSF;

class CRMRuleTable : public JXStringTable
{
public:

	enum
	{
		kFirstColumn   = 1,
		kRestColumn    = 2,
		kReplaceColumn = 3
	};

public:

	CRMRuleTable(EditCRMDialog* dialog,
				   JXTextButton* addRowButton, JXTextButton* removeRowButton,
				   JXTextButton* loadButton, JXTextButton* saveButton,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	~CRMRuleTable() override;

	void	Activate() override;
	void	Deactivate() override;

	void	GetData(JStyledText::CRMRuleList* list) const;
	void	SetData(const JStyledText::CRMRuleList& list);
	void	ClearData();

	void	ReadGeometry(std::istream& input);
	void	WriteGeometry(std::ostream& output) const;
	void	SetColTitles(JXColHeaderWidget* widget) const;

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

	void	PrepareDeleteXInputField() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	EditCRMDialog*	itsDialog;
	JXTextButton*	itsAddRowButton;
	JXTextButton*	itsRemoveRowButton;

	ListCSF*		itsCSF;
	JXTextButton*	itsLoadButton;
	JXTextButton*	itsSaveButton;

	// for testing input fields

	JRegex*			itsFirstRegex;
	JRegex*			itsRestRegex;
	JInterpolate*	itsReplaceInterpolator;

private:

	void	AddRow();
	void	RemoveRow();

	void	LoadRules();
	void	SaveRules() const;

	void	ReadData(const JString& fileName, const bool replace);
	void	WriteData(const JString& fileName) const;
};

#endif
