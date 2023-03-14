/******************************************************************************
 TreeWidget.h

	Interface for the TreeWidget class

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_TreeWidget
#define _H_TreeWidget

#include <jx-af/jx/JXScrollableWidget.h>

class TreeDirector;
class Tree;
class Class;
class JPagePrinter;
class JEPSPrinter;

class TreeWidget : public JXScrollableWidget
{
public:

	static const JSize	kBorderWidth;

public:

	TreeWidget(TreeDirector* director, Tree* tree,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~TreeWidget() override;

	Tree*	GetTree() const;

	bool	FindClass(const JString& name,
					  const JXMouseButton button = kJXRightButton,
					  const bool raiseTreeWindow = false,
					  const bool reportNotFound = true,
					  const bool openFileIfSingleMatch = true,
					  const bool deselectAll = true) const;
	bool	FindFunction(const JString& fnName,
						 const JXMouseButton button = kJXRightButton,
						 const bool raiseTreeWindow = false,
						 const bool reportNotFound = true,
						 const bool openFileIfSingleMatch = true,
						 const bool deselectAll = true) const;
	bool	FindFunction(const JArray<JIndex>& symbolList,
						 const JXMouseButton button = kJXRightButton,
						 const bool raiseTreeWindow = false,
						 const bool reportNotFound = true,
						 const bool openFileIfSingleMatch = true,
						 const bool deselectAll = true) const;

	void	Print(JPagePrinter& p);
	void	Print(JEPSPrinter& p);

	void	ReadSetup(std::istream& setInput, const JFileVersion setVers);
	void	WriteSetup(std::ostream& setOutput) const;

	static bool	WillRaiseWindowWhenSingleMatch();
	static void	ShouldRaiseWindowWhenSingleMatch(const bool raise);

	void	HandleKeyPress(const JUtf8Character& c,
						   const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	bool	HitSamePart(const JPoint& pt1, const JPoint& pt2) const override;

	void	GetSelectionData(JXSelectionData* data,
							 const JString& id) override;
	Atom	GetDNDAction(const JXContainer* target,
						 const JXButtonStates& buttonStates,
						 const JXKeyModifiers& modifiers) override;
	void	HandleDNDResponse(const JXContainer* target,
							  const bool dropAccepted, const Atom action) override;

	bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
						   const JPoint& pt, const Time time,
						   const JXWidget* source) override;
	void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
						  const Atom action, const Time time,
						  const JXWidget* source) override;

	void	HandleFocusEvent() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	TreeDirector*	itsDirector;		// owns us
	Tree*			itsTree;			// not owned
	JString			itsKeyBuffer;

	bool			itsExpectDragFlag;
	JPoint			itsStartPt;

	static bool	itsRaiseWhenSingleMatchFlag;

private:

	bool	ShowSearchResults(const std::function<void(Class*)>& singleResultAction,
							  const JXMouseButton button,
							  const bool raiseTreeWindow,
							  const bool reportNotFound,
							  const bool openFileIfSingleMatch,
							  const bool deselectAll,
							  const JUtf8Byte* notFoundID) const;
};


/******************************************************************************
 GetTree

 ******************************************************************************/

inline Tree*
TreeWidget::GetTree()
	const
{
	return itsTree;
}

/******************************************************************************
 Raise window when single match (static)

 ******************************************************************************/

inline bool
TreeWidget::WillRaiseWindowWhenSingleMatch()
{
	return itsRaiseWhenSingleMatchFlag;
}

inline void
TreeWidget::ShouldRaiseWindowWhenSingleMatch
	(
	const bool raise
	)
{
	itsRaiseWhenSingleMatchFlag = raise;
}

#endif
