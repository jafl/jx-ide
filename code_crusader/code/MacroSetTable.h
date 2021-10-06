/******************************************************************************
 MacroSetTable.h

	Interface for the MacroSetTable class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_MacroSetTable
#define _H_MacroSetTable

#include <jx-af/jx/JXStringTable.h>
#include "PrefsManager.h"		// need definition of MacroSetInfo

class JXTextButton;
class CharActionTable;
class MacroTable;

class MacroSetTable : public JXStringTable
{
public:

	MacroSetTable(JArray<PrefsManager::MacroSetInfo>* macroList,
					const JIndex initialSelection, const JIndex firstUnusedID,
					CharActionTable* actionTable, MacroTable* macroTable,
					JXTextButton* addRowButton, JXTextButton* removeRowButton,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~MacroSetTable();

	bool	ContentsValid() const;
	bool	GetCurrentMacroSetName(JString* name) const;

	JArray<PrefsManager::MacroSetInfo>*
		GetMacroList(JIndex* firstNewID, JIndex* lastNewID) const;

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

	JArray<PrefsManager::MacroSetInfo>*	itsMacroList;

	mutable bool	itsOwnsMacroListFlag;	// true => delete contents of itsMacroList
	const JIndex		itsFirstNewID;			// first index to use for new sets
	JIndex				itsLastNewID;			// index of last new set created
	JIndex				itsMacroIndex;			// index of currently displayed macro set

	CharActionTable*	itsActionTable;
	MacroTable*		itsMacroTable;

	JXTextButton*		itsAddRowButton;
	JXTextButton*		itsRemoveRowButton;

private:

	void	AddRow();
	void	RemoveRow();
	void	SwitchDisplay();

	// not allowed

	MacroSetTable(const MacroSetTable& source);
	const MacroSetTable& operator=(const MacroSetTable& source);
};

#endif
