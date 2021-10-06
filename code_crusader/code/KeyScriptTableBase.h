/******************************************************************************
 KeyScriptTableBase.h

	Interface for the KeyScriptTableBase class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_KeyScriptTableBase
#define _H_KeyScriptTableBase

#include <jx-af/jx/JXStringTable.h>

class JXTextButton;
class EditMacroDialog;

class KeyScriptTableBase : public JXStringTable
{
public:

	enum
	{
		kMacroColumn  = 1,
		kScriptColumn = 2
	};

public:

	KeyScriptTableBase(EditMacroDialog* dialog,
						 JXTextButton* addRowButton, JXTextButton* removeRowButton,
						 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						 const HSizingOption hSizing, const VSizingOption vSizing,
						 const JCoordinate x, const JCoordinate y,
						 const JCoordinate w, const JCoordinate h);

	virtual ~KeyScriptTableBase();

	void	Activate() override;
	void	Deactivate() override;

	bool	ContentsValid() const;

protected:

	EditMacroDialog*	GetDialog() const;

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
	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	EditMacroDialog*	itsDialog;
	JXTextButton*		itsAddRowButton;
	JXTextButton*		itsRemoveRowButton;

private:

	void	AddRow();
	void	RemoveRow();

	void	AdjustColWidths();

	// not allowed

	KeyScriptTableBase(const KeyScriptTableBase& source);
	const KeyScriptTableBase& operator=(const KeyScriptTableBase& source);
};


/******************************************************************************
 GetDialog (protected)

 ******************************************************************************/

inline EditMacroDialog*
KeyScriptTableBase::GetDialog()
	const
{
	return itsDialog;
}

#endif
