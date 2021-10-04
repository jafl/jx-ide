/******************************************************************************
 PathTable.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_PathTable
#define _H_PathTable

#include <jx-af/jx/JXEditTable.h>

class JString;
class JStringTableData;
class JXImage;
class JXTextButton;
class JXPathInput;
class DirList;
class RelPathCSF;

class PathTable : public JXEditTable
{
public:

	PathTable(const DirList& pathList,
				JXTextButton* addPathButton, JXTextButton* removePathButton,
				JXTextButton* choosePathButton, RelPathCSF* csf,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~PathTable();

	void	AddDirectories(const JPtrArray<JString>& list);
	void	GetPathList(DirList* pathList) const;

	virtual bool	IsEditable(const JPoint& cell) const override;

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual bool		ExtractInputData(const JPoint& cell) override;
	virtual void			PrepareDeleteXInputField() override;

	virtual bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source) override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JStringTableData*	itsData;
	JXImage*			itsFolderIcon;			// not owned
	JXPathInput*		itsPathInput;			// nullptr unless editing
	JString				itsBasePath;

	JXTextButton*		itsAddPathButton;		// not owned
	JXTextButton*		itsRemovePathButton;	// not owned
	JXTextButton*		itsChoosePathButton;	// not owned
	RelPathCSF*		itsCSF;					// not owned

private:

	void	ChoosePath();
	void	UpdateButtons();

	// not allowed

	PathTable(const PathTable& source);
	const PathTable& operator=(const PathTable& source);
};

#endif