/******************************************************************************
 CPPMacroTable.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CPPMacroTable
#define _H_CPPMacroTable

#include <jx-af/jx/JXStringTable.h>

class JXTextButton;
class JXColHeaderWidget;
class CPreprocessor;

class CPPMacroTable : public JXStringTable
{
public:

	CPPMacroTable(const CPreprocessor& cpp,
					JXTextButton* addRowButton, JXTextButton* removeRowButton,
					JXTextButton* loadButton, JXTextButton* saveButton,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~CPPMacroTable() override;

	bool	ContentsValid() const;
	bool	UpdateMacros(CPreprocessor* cpp) const;

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;
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
	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTextButton*	itsAddRowButton;
	JXTextButton*	itsRemoveRowButton;

	mutable JString	itsFileName;
	JXTextButton*	itsLoadButton;
	JXTextButton*	itsSaveButton;

private:

	void	AddRow();
	void	RemoveRow();

	void	LoadMacros();
	void	SaveMacros() const;

	void	ReadData(const JString& fileName, const bool replace);
	void	WriteData(const JString& fileName) const;

	void	AdjustColWidths();
};

#endif
