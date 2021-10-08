/******************************************************************************
 EditCommandsTable.h

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_EditCommandsTable
#define _H_EditCommandsTable

#include <jx-af/jx/JXEditTable.h>

class JXTextButton;
class EditCommandsDialog;

class EditCommandsTable : public JXEditTable
{
public:

	EditCommandsTable(EditCommandsDialog* dialog, JXTextButton* removeButton,
						JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	~EditCommandsTable() override;

	void	SyncWithData();
	void	NewStringAppended();
	void	AdjustColWidth();
	void	RemoveCurrent();

protected:

	bool			ExtractInputData(const JPoint& cell) override;
	JXInputField*	CreateXInputField(const JPoint& cell,
									  const JCoordinate x, const JCoordinate y,
									  const JCoordinate w, const JCoordinate h) override;
	void			PrepareDeleteXInputField() override;

	void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	void			HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void			ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	EditCommandsDialog*	itsDialog;
	JCoordinate			itsMinColWidth;
	JXInputField*		itsCmdInput;
	JXTextButton*		itsRemoveButton;
};

#endif
