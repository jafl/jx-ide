/******************************************************************************
 SymbolTable.h

	Interface for the SymbolTable class

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_SymbolTable
#define _H_SymbolTable

#include <jx-af/jx/JXTable.h>

class JError;
class JString;
class JRegex;
class JXImage;
class SymbolDirector;
class SymbolList;

class SymbolTable : public JXTable
{
public:

	SymbolTable(SymbolDirector* symbolDirector, SymbolList* symbolList,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	~SymbolTable() override;

	bool	HasSelection() const;
	void		SelectSingleEntry(const JIndex index, const bool scroll = true);
	void		ClearSelection();
	void		CopySelectedSymbolNames() const;
	void		DisplaySelectedSymbols() const;
	void		FindSelectedSymbols(const JXMouseButton button) const;
	void		GetFileNamesForSelection(JPtrArray<JString>* fileNameList,
										 JArray<JIndex>* lineIndexList) const;

	void	ShowAll();
	JError	SetNameFilter(const JString& filterStr, const bool isRegex);
	void	SetDisplayList(const JArray<JIndex>& list);

	void	HandleKeyPress(const JUtf8Character& c,
						   const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers) override;

	void	HandleFocusEvent() override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	SymbolDirector*	itsSymbolDirector;			// not owned
	SymbolList*		itsSymbolList;				// not owned
	JArray<JIndex>*	itsVisibleList;				// index of each visible symbol
	bool			itsVisibleListLockedFlag;	// true => RebuildTable() doesn't change it
	JRegex*			itsNameFilter;				// nullptr if not used
	JString*		itsNameLiteral;				// nullptr if not used

	JSize			itsMaxStringWidth;
	mutable JString	itsKeyBuffer;

private:

	void	RebuildTable();
	bool	CalcColWidths(const JString& symbolName, const JString* signature);
	void	AdjustColWidths();

	JIndex	CellToSymbolIndex(const JPoint& cell) const;
};

#endif
