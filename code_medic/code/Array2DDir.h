/******************************************************************************
 Array2DDir.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_Array2DDir
#define _H_Array2DDir

#include <jx-af/jx/JXWindowDirector.h>
#include "MemoryDir.h"	// need defn of DisplayType
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JRange.h>
#include <jx-af/jcore/JPoint.h>

class JStringTableData;
class JXTextButton;
class JXTextMenu;
class JXRowHeaderWidget;
class JXColHeaderWidget;
class Link;
class Array2DTable;
class Array2DCmd;
class ArrayExprInput;
class ArrayIndexInput;
class CommandDirector;

class Array2DDir : public JXWindowDirector
{
public:

	Array2DDir(CommandDirector* supervisor, const JString& expr);
	Array2DDir(std::istream& input, const JFileVersion vers,
				 CommandDirector* supervisor);

	virtual	~Array2DDir();

	const JString&	GetExpression() const;
	bool		ArrayRowIndexValid(const JInteger index) const;
	bool		ArrayColIndexValid(const JInteger index) const;

	void	Display1DArray();
	void	Plot1DArray();
	void	Display2DArray();
	void	WatchExpression();
	void	WatchLocation();
	void	ExamineMemory(const MemoryDir::DisplayType type);

	virtual void		Activate() override;
	virtual bool	GetMenuIcon(const JXImage** icon) const override;

	void	StreamOut(std::ostream& output);

	// called by Array2DCmd

	void	UpdateNext();
	JString	GetExpression(const JPoint& cell) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	Link*				itsLink;
	CommandDirector*	itsCommandDir;
	JStringTableData*	itsData;
	JString				itsExpr;
	JIntRange			itsRowDisplayRange;
	JIntRange			itsRowRequestRange;
	JIntRange			itsColDisplayRange;
	JIntRange			itsColRequestRange;
	bool			itsShouldUpdateFlag;
	JArray<JIntRange>*	itsRowUpdateList;
	JArray<JIntRange>*	itsColUpdateList;
	Array2DCmd*	itsUpdateCmd;
	bool			itsWaitingForReloadFlag;

	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsActionMenu;
	JXTextMenu*			itsHelpMenu;

// begin JXLayout

	ArrayExprInput*  itsExprInput;
	ArrayIndexInput* itsRowStartIndex;
	ArrayIndexInput* itsRowEndIndex;
	ArrayIndexInput* itsColStartIndex;
	ArrayIndexInput* itsColEndIndex;

// end JXLayout

// begin tableLayout

	Array2DTable*      itsTable;
	JXColHeaderWidget* itsColHeader;
	JXRowHeaderWidget* itsRowHeader;

// end tableLayout

private:

	void	Array2DDirX(CommandDirector* supervisor);
	void	BuildWindow();
	void	UpdateWindowTitle();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	void	FitToRanges();
	void	SetRowTitles(const JIntRange& r);
	void	SetColTitles(const JIntRange& r);

	void	ShouldUpdate(const bool update);
	void	UpdateAll();
	void	BeginRowUpdate(const JIntRange& r);
	void	BeginColUpdate(const JIntRange& r);
	void	BeginUpdate(JArray<JIntRange>* list, const JIntRange& range);
};


/******************************************************************************
 GetExpression

 ******************************************************************************/

inline const JString&
Array2DDir::GetExpression()
	const
{
	return itsExpr;
}

/******************************************************************************
 Array index valid

 ******************************************************************************/

inline bool
Array2DDir::ArrayRowIndexValid
	(
	const JInteger index
	)
	const
{
	return itsRowDisplayRange.Contains(index);
}

inline bool
Array2DDir::ArrayColIndexValid
	(
	const JInteger index
	)
	const
{
	return itsColDisplayRange.Contains(index);
}

/******************************************************************************
 BeginRowUpdate (private)

 ******************************************************************************/

inline void
Array2DDir::BeginRowUpdate
	(
	const JIntRange& range
	)
{
	BeginUpdate(itsRowUpdateList, range);
}

/******************************************************************************
 BeginColUpdate (private)

 ******************************************************************************/

inline void
Array2DDir::BeginColUpdate
	(
	const JIntRange& range
	)
{
	BeginUpdate(itsColUpdateList, range);
}

#endif
