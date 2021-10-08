/******************************************************************************
 DiffStyleMenu.h

	Interface for the DiffStyleMenu class

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_DiffStyleMenu
#define _H_DiffStyleMenu

#include <jx-af/jx/JXStyleMenu.h>

class DiffStyleMenu : public JXStyleMenu
{
public:

	DiffStyleMenu(const JString& title, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~DiffStyleMenu();

	const JFontStyle&	GetStyle() const;
	void				SetStyle(const JFontStyle& style);

	void	ReadStyle(std::istream& input);
	void	WriteStyle(std::ostream& output);

protected:

	JFontStyle	GetFontStyleForMenuUpdate() const override;
	void		HandleMenuItem(const JIndex menuItem) override;

private:

	JFontStyle	itsStyle;
};


/******************************************************************************
 GetStyle

 ******************************************************************************/

inline const JFontStyle&
DiffStyleMenu::GetStyle()
	const
{
	return itsStyle;
}

/******************************************************************************
 SetStyle

 ******************************************************************************/

inline void
DiffStyleMenu::SetStyle
	(
	const JFontStyle& style
	)
{
	itsStyle = style;
}

#endif
