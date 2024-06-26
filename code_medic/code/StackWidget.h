/******************************************************************************
 StackWidget.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_StackWidget
#define _H_StackWidget

#include <jx-af/jx/JXNamedTreeListWidget.h>

class JTree;
class JXIdleTask;
class Link;
class CommandDirector;
class StackDir;
class GetStackCmd;
class GetFrameCmd;
class StackFrameNode;

class StackWidget : public JXNamedTreeListWidget
{
public:

	StackWidget(CommandDirector* commandDir,
				  StackDir* stackDir, JTree* tree, JNamedTreeList* treeList,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	~StackWidget() override;

	void	Update();
	void	SelectFrame(const JUInt64 id);
	bool	GetStackFrame(const JUInt64 id, const StackFrameNode** frame);
	void	SwitchToFrame(const JUInt64 id);

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

	// void called by GetStackCmd

	void	FinishedLoading(const JIndex initID);

protected:

	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	JSize	GetMinCellWidth(const JPoint& cell) const override;

	void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	Link*				itsLink;
	CommandDirector*	itsCommandDir;
	StackDir*			itsStackDir;
	JTree*				itsTree;
	GetStackCmd*		itsGetStackCmd;
	GetFrameCmd*		itsGetFrameCmd;
	bool				itsNeedsUpdateFlag;
	bool				itsSmartFrameSelectFlag;
	bool				itsIsWaitingForReloadFlag;
	bool				itsChangingFrameFlag;
	bool				itsSelectingFrameFlag;
	JXIdleTask*			itsFlushStateTask;

private:

	bool	SelectNextFrame(const JInteger delta);

	void	FlushOldData();
	bool	ShouldRebuild() const;
	void	Rebuild();
};

#endif
