/******************************************************************************
 StylerTable.h

	Interface for the StylerTable class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_StylerTable
#define _H_StylerTable

#include <jx-af/jx/JXStringTable.h>
#include <jx-af/jcore/JStringMap.h>
#include "StylerBase.h"		// need definition of WordStyle

class JXTextButton;
class StylerTableMenu;

class StylerTable : public JXStringTable
{
public:

	StylerTable(const JUtf8Byte** typeNames,
				  const JArray<JFontStyle>& typeStyles,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	StylerTable(const TextFileType fileType,
				  const JArray<StylerBase::WordStyle>& wordList,
				  JXTextButton* addRowButton, JXTextButton* removeButton,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~StylerTable();

	void	GetData(JArray<JFontStyle>* typeStyles) const;
	void	GetData(JStringMap<JFontStyle>* wordStyles) const;

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

	// called by StylerTableInput

	void	DisplayFontMenu(const JPoint& cell, JXContainer* mouseOwner,
							const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers);

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h) override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	TextFileType		itsFileType;
	bool				itsAllowEditFlag;
	StylerTableMenu*	itsStyleMenu;
	JXTextButton*		itsAddRowButton;	// can be nullptr
	JXTextButton*		itsRemoveButton;	// can be nullptr

private:

	void	StylerTableX(const TextFileType fileType, const bool allowEdit,
						   JXTextButton* addRowButton, JXTextButton* removeButton);
	void	UpdateButtons();

	void	AddRow();
	void	RemoveSelection();

	// not allowed

	StylerTable(const StylerTable& source);
	const StylerTable& operator=(const StylerTable& source);
};

#endif
