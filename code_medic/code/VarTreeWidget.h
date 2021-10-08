/******************************************************************************
 VarTreeWidget.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_VarTreeWidget
#define _H_VarTreeWidget

#include <jx-af/jx/JXNamedTreeListWidget.h>
#include "MemoryDir.h"	// need defn of DisplayType

class JTree;
class JNamedTreeList;
class JXMenuBar;
class JXTextMenu;
class CommandDirector;
class VarNode;

class VarTreeWidget : public JXNamedTreeListWidget
{
public:

	VarTreeWidget(CommandDirector* dir,
					const bool mainDisplay, JXMenuBar* menuBar,
					JTree* tree, JNamedTreeList* treeList,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~VarTreeWidget();

	VarNode*	NewExpression(const JString& expr = JString::empty);
	VarNode*	DisplayExpression(const JString& expr);
	void		ShowNode(const VarNode* node);

	bool	HasSelection() const;
	void	RemoveSelection();

	void	WatchExpression();
	void	WatchLocation();
	void	DisplayAsCString();
	void	Display1DArray();
	void	Plot1DArray();
	void	Display2DArray();
	void	ExamineMemory(const MemoryDir::DisplayType type);
	void	ShouldUpdate(const bool update);

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;
	bool	IsEditable(const JPoint& cell) const override;

	void	ReadSetup(std::istream& input, const JFileVersion vers);
	void	WriteSetup(std::ostream& output) const;

protected:

	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	JSize	GetMinCellWidth(const JPoint& cell) const override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	bool	ExtractInputData(const JPoint& cell) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	enum DragType
	{
		kInvalidDrag,
		kSelectDrag,
		kOpenNodeDrag
	};

private:

	CommandDirector*	itsDir;
	JTree*				itsTree;
	const bool			itsIsMainDisplayFlag;
	bool				itsWaitingForReloadFlag;
	DragType			itsDragType;

	JXTextMenu*	itsEditMenu;			// not owned
	JIndex		itsCopyPathCmdIndex;
	JIndex		itsCopyValueCmdIndex;

	JXTextMenu*	itsBaseMenu;
	JXTextMenu*	itsBasePopupMenu;

	// used while editing

	bool	itsEditingNewNodeFlag;
	JString	itsOrigEditValue;

private:

	void	FlushOldData();

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex item);

	void	CopySelectedItems(const bool useFullName, const bool copyValue) const;

	void	UpdateBaseMenu(JXTextMenu* menu);
	void	HandleBaseMenu(const JIndex item);
};

#endif
