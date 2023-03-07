/******************************************************************************
 ProjectTable.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_ProjectTable
#define _H_ProjectTable

#include <jx-af/jx/JXNamedTreeListWidget.h>

class JXImage;
class JXMenuBar;
class JXTextMenu;
class JXImageButton;
class JXGetStringDialog;
class ProjectDocument;
class ProjectNode;
class GroupNode;

class ProjectTable : public JXNamedTreeListWidget
{
public:

	enum	// selection depth
	{
		kEitherDepth = 0,
		kGroupDepth  = 1,
		kFileDepth   = 2
	};

	enum SelType
	{
		kNoSelection,
		kFileSelection,
		kGroupSelection
	};

	// Do not change these values once they are assigned because
	// they are stored in the prefs file.

	enum PathType
	{
		kAbsolutePath = 1,	// avoid conflict with X atom None when calling ChooseDropAction()
		kProjectRelative,
		kHomeRelative
	};

	// Do not change these values once they are assigned because
	// they are stored in the prefs file.

	enum DropFileAction
	{
		kDropAskPathType     = 0,
		kDropAbsolutePath    = kAbsolutePath,
		kDropProjectRelative = kProjectRelative,
		kDropHomeRelative    = kHomeRelative
	};

	// action to take after file path/name has been edited

	enum InputAction
	{
		kRelink,
		kRename
	};

public:

	ProjectTable(ProjectDocument* doc, JXMenuBar* menuBar,
				   JNamedTreeList* treeList,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	~ProjectTable() override;

	bool	NewGroup(GroupNode** returnNode = nullptr);
	void	AddDirectoryTree();
	void	AddDirectoryTree(const JString& fullPath, const PathType pathType);
	void	AddFiles();
	bool	AddFiles(const JPtrArray<JString>& fullNameList,
					 const PathType pathType,
					 const bool updateProject = true,
					 const bool silent = false);

	static JString	ConvertToAbsolutePath(const JString& origName,
										  const JString& projPath,
										  PathType* pathType);
	static JString	ConvertToRelativePath(const JString& fullPath,
										  const JString& projPath,
										  const PathType pathType);
	static PathType	CalcPathType(const JString& path);

	bool	HasSelection() const;
	bool	GetSelectionType(SelType* type, bool* single, JIndex* index) const;
	void	SelectAll();
	void	ClearSelection();
	void	SelectFileNodes(const JPtrArray<JTreeNode>& nodeList);
	void	OpenSelection();
	void	OpenComplementFiles();
	void	RemoveSelection();
	void	PlainDiffSelection();
	void	VCSDiffSelection();
	void	ShowSelectedFileLocations();
	void	GetSelectedFileNames(JPtrArray<JString>* list) const;
	void	EditFilePath();
	void	EditSubprojectConfig();

	ProjectNode*		GetProjectNode(const JIndex index);
	const ProjectNode*	GetProjectNode(const JIndex index) const;

	void	ReadSetup(std::istream& projInput, const JFileVersion projVers,
					  std::istream* setInput, const JFileVersion setVers);
	void	WriteSetup(std::ostream& projOutput, std::ostream* setOutput) const;

	static DropFileAction	GetDropFileAction();
	static void				SetDropFileAction(const DropFileAction action);

	void	HandleKeyPress(const JUtf8Character& c,
						   const int keySym, const JXKeyModifiers& modifiers) override;
	bool	IsSelectable(const JPoint& cell,
						 const bool forExtend) const override;

	void	SetInputAction(const InputAction action);

protected:

	JSize	GetDepth(const JIndex index) const;

	bool	GetImage(const JIndex index, const JXImage** image) const override;
	void	AdjustToTree() override;

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	void	DNDInit(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
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
	void		HandleDNDEnter() override;
	void		HandleDNDHere(const JPoint& pt, const JXWidget* source) override;
	void		HandleDNDLeave() override;
	void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source) override;

	JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	bool		ExtractInputData(const JPoint& cell) override;
	void			PrepareDeleteXInputField() override;

	JXInputField*
		CreateTreeListInput(const JPoint& cell, JXContainer* enclosure,
							const HSizingOption hSizing, const VSizingOption vSizing,
							const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum DragType
	{
		kInvalidDrag,
		kSelectDrag,
		kWaitForDNDDrag,
		kOpenNodeDrag
	};

	enum DNDAction
	{
		kDNDInvalid,
		kDNDFileBombsight,		// line at bottom of itsDNDCell
		kDNDAppendToGroup,		// frame itsDNDCell
		kDNDNewGroup,			// line at bottom of itsDNDCell; table may be empty
		kDNDGroupBombsight		// line at top of itsDNDCell; may be beyond bottom of table
	};

private:

	ProjectDocument*	itsDoc;				// not owned
	JXTextMenu*			itsEditMenu;		// not owned
	JXTextMenu*			itsContextMenu;		// not owned
	JXImageButton*		itsCSFButton;		// nullptr unless editing FileNodeBase

	JSize	itsSelDepth;				// depth of items that can be selected
	bool	itsIgnoreSelChangesFlag;	// true while cleaning selection
	bool	itsMarkWritableFlag;		// true if mark writable files (e.g. CVS)
	bool	itsLockedSelDepthFlag;		// true if don't change the selection depth

	// used after editing

	InputAction	itsInputAction;

	// used while dragging

	DragType	itsDragType;
	JPoint		itsStartPt;

	// used during DND

	DNDAction	itsDNDAction;
	JPoint		itsDNDCell;
	JString*	itsDNDBuffer;

	static DropFileAction	itsDropFileAction;

private:

	void	CleanSelection();
	void	UpdateSelDepth();

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex index);
	void	CopySelectedNames() const;

	void	CreateContextMenu();
	void	UpdateContextMenu();
	void	HandleContextMenu(const JIndex index);

	void	CopyFileToDNDList(const JTreeNode* node, JPtrArray<JString>* fileList) const;

	void	InsertFileSelectionAfter(JTreeNode* after);
	void	AppendFileSelectionToGroup(JTreeNode* group);
	void	InsertGroupSelectionBefore(JTreeNode* before);
	void	InsertExtDroppedFiles(const JPtrArray<JString>& fileNameList,
								  const PathType pathType);

	void	AddDirectoryTree(const JString& fullPath, const JString& filterStr,
							 const PathType pathType);
	void	AddDirectoryTree(const JString& fullPath, const JString& relPath,
							 const JString& filterStr,
							 const PathType pathType,
							 bool* changed);
};

std::istream& operator>>(std::istream& input, ProjectTable::DropFileAction& action);
std::ostream& operator<<(std::ostream& output, const ProjectTable::DropFileAction action);


/******************************************************************************
 Action when files are dropped from external source (static)

 ******************************************************************************/

inline ProjectTable::DropFileAction
ProjectTable::GetDropFileAction()
{
	return itsDropFileAction;
}

inline void
ProjectTable::SetDropFileAction
	(
	const DropFileAction action
	)
{
	itsDropFileAction = action;
}

/******************************************************************************
 SetInputAction

 ******************************************************************************/

inline void
ProjectTable::SetInputAction
	(
	const InputAction action
	)
{
	itsInputAction = action;
}

#endif
