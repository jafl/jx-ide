/******************************************************************************
 Array2DTable.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_Array2DTable
#define _H_Array2DTable

#include <jx-af/jx/JXStringTable.h>

class JXMenuBar;
class JXTextMenu;
class CommandDirector;
class Array2DDir;

class Array2DTable : public JXStringTable
{
public:

	Array2DTable(CommandDirector* cmdDir,
				   Array2DDir* arrayDir, JXMenuBar* menuBar,
				   JStringTableData* data,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	~Array2DTable() override;

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	JXInputField*	CreateXInputField(const JPoint& cell,
									  const JCoordinate x, const JCoordinate y,
									  const JCoordinate w, const JCoordinate h) override;
	bool			ExtractInputData(const JPoint& cell) override;

	JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const override;
	void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CommandDirector*	itsCmdDir;
	Array2DDir*			itsArrayDir;
	JXTextMenu*			itsEditMenu;
	JString				itsOrigEditValue;

private:

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex index);
};

#endif
