/******************************************************************************
 StylerTableMenu.h

	Interface for the StylerTableMenu class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_StylerTableMenu
#define _H_StylerTableMenu

#include <jx-af/jx/JXStyleMenu.h>

class JXStyleTable;

class StylerTableMenu : public JXStyleMenu
{
public:

	StylerTableMenu(JXStyleTable* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~StylerTableMenu();

	void	PrepareForPopup(const JPoint& cell);

protected:

	virtual JFontStyle	GetFontStyleForMenuUpdate() const;
	virtual void		HandleMenuItem(const JIndex menuItem);

private:

	JXStyleTable*	itsTable;		// not owned
	JPoint			itsPopupCell;	// cell that was clicked to pop up menu

private:

	void	UpdateStyle(const JIndex index, const JFontStyle& origStyle,
						JFontStyle* style);

	// not allowed

	StylerTableMenu(const StylerTableMenu& source);
	const StylerTableMenu& operator=(const StylerTableMenu& source);
};

/******************************************************************************
 PrepareForPopup

 ******************************************************************************/

inline void
StylerTableMenu::PrepareForPopup
	(
	const JPoint& cell
	)
{
	itsPopupCell = cell;
}

#endif
