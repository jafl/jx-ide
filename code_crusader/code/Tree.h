/******************************************************************************
 Tree.h

	Interface for the Tree Class

	Copyright © 1995-99 John Lindal.

 ******************************************************************************/

#ifndef _H_Tree
#define _H_Tree

#include <jx-af/jcore/JContainer.h>
#include "TextFileType.h"
#include "PrefsManager.h"		// need definition of FileTypesChanged
#include <jx-af/jcore/JPtrArray.h>
#include <jx-af/jcore/JRect.h>
#include <jx-af/jcore/JFAID.h>
#include <jx-af/jcore/jTime.h>

class JString;
class JPainter;
class JFontManager;
class JProgressDisplay;
class Class;
class DirList;
class TreeDirector;
class ProjectDocument;

class Tree;
using ClassStreamInFn = Class* (*)(std::istream& input, const JFileVersion vers, Tree* tree);

class Tree : public JContainer
{
	friend class Class;

public:

	Tree(ClassStreamInFn streamInFn,
		   TreeDirector* director, const TextFileType fileType,
		   const JSize marginWidth);
	Tree(std::istream& projInput, const JFileVersion projVers,
		   std::istream* setInput, const JFileVersion setVers,
		   std::istream* symInput, const JFileVersion symVers,
		   ClassStreamInFn streamInFn,
		   TreeDirector* director, const TextFileType fileType,
		   const JSize marginWidth, DirList* dirList);

	~Tree() override;

	void	NextUpdateMustReparseAll();
	bool	NeedsReparseAll() const;
	void	RebuildLayout();

	bool	FindClass(const JString& fullName, Class** theClass) const;
	bool	FindClass(const JString& fullName, const Class** theClass) const;
	bool	IsUniqueClassName(const JString& name, const Class** theClass) const;
	bool	ClosestVisibleMatch(const JString& prefixStr, Class** theClass) const;
	bool	FindParent(const JString& parentName, const Class* container,
					   Class** parent, JString* nameSpace) const;

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const DirList* dirList) const;

	bool	HasSelection() const;
	void	DeselectAll();
	bool	GetSelectedClasses(JPtrArray<Class>* classList) const;
	bool	GetSelectionCoverage(JRect* coverage, JSize* count) const;
	void	SelectClasses(const JString& name, const bool deselectAll = true);
	void	SelectImplementors(const JString& fnName, const bool caseSensitive,
							   const bool deselectAll = true);
	void	SelectParents();
	void	SelectDescendants();
	void	CollapseExpandSelectedClasses(const bool collapse);
	void	ExpandAllClasses();

	void	ViewSelectedSources() const;
	void	ViewSelectedHeaders() const;
	void	ViewSelectedFunctionLists() const;
	void	CopySelectedClassNames() const;
	void	DeriveFromSelected() const;

	bool	WillAutoMinimizeMILinks() const;
	void	ShouldAutoMinimizeMILinks(const bool autoMinimize);
	bool	NeedsMinimizeMILinks() const;		// when auto-minimize is off or cancelled
	void	ForceMinimizeMILinks();

	void	GetMenuInfo(bool* hasSelection,
						bool* canCollapse, bool* canExpand) const;
	bool	GetClass(const JPoint& pt, Class** theClass) const;
	bool	HitSameClass(const JPoint& pt1, const JPoint& pt2, Class** theClass) const;

	void	Draw(JPainter& p, const JRect& rect) const;

	bool	WillDrawMILinksOnTop() const;
	void	ShouldDrawMILinksOnTop(const bool drawOnTop);

	JFontManager*	GetFontManager() const;
	JSize			GetFontSize() const;
	void			SetFontSize(const JSize size);
	JCoordinate		GetLineHeight() const;
	void			GetBounds(JCoordinate* width, JCoordinate* height) const;

	const JPtrArray<JString>&	GetParseSuffixes() const;
	const JPtrArray<Class>&		GetClasses() const;
	const JPtrArray<Class>&		GetVisibleClasses() const;

	ProjectDocument*	GetProjectDoc() const;
	TreeDirector*		GetTreeDirector() const;
	TextFileType		GetFileType() const;

	// for loading updated symbols

	virtual void	ReloadSetup(std::istream& input, const JFileVersion vers);

	// called by TreeDirector

	void	FileTypesChanged(const PrefsManager::FileTypesChanged& info);

	virtual void	PrepareForUpdate(const bool reparseAll);
	virtual bool	UpdateFinished(const JArray<JFAID_t>& deadFileList);

	// called by FileListTable

	void	FileChanged(const JString& fileName,
						const TextFileType fileType, const JFAID_t id);

protected:

	struct RootGeom
	{
		Class*	root;	// not owned
		JSize		top;
		JSize		h;

		RootGeom()
			:
			root(nullptr), top(0), h(0)
		{ };

		RootGeom(Class* r, const JSize t, const JSize height)
			:
			root(r), top(t), h(height)
		{ };
	};

protected:

	JPtrArray<Class>*	GetClasses();
	JPtrArray<Class>*	GetVisibleClasses();

	void	PlaceAll(JArray<RootGeom>* rootGeom = nullptr);

	virtual void	ParseFile(const JString& fileName, const JFAID_t id) = 0;

private:

	struct RootConn
	{
		JSize	dy;
		JIndex	otherRoot;

		RootConn()
			:
			dy(0), otherRoot(0)
		{ };

		RootConn(const JSize deltaY, const JIndex i)
			:
			dy(deltaY), otherRoot(i)
		{ };
	};

	struct RootMIInfo
	{
		Class*			root;		// not owned
		JSize				h;
		JArray<RootConn>*	connList;

		RootMIInfo()
			:
			root(nullptr), h(0), connList(nullptr)
		{ };

		RootMIInfo(Class* r, const JSize height, JArray<RootConn>* list)
			:
			root(r), h(height), connList(list)
		{ };
	};

	struct RootSubset
	{
		JArray<bool>*	content;
		JArray<JIndex>*		order;
		JSize				linkLength;

		RootSubset()
			:
			content(nullptr), order(nullptr), linkLength(0)
		{ };

		RootSubset(JArray<bool>* c, JArray<JIndex>* o, const JSize l)
			:
			content(c), order(o), linkLength(l)
		{ };
	};

private:

	TreeDirector*		itsDirector;				// not owned

	JPtrArray<Class>*	itsClassesByFull;			// owns objects
	JPtrArray<Class>*	itsVisibleByGeom;			// doesn't own objects
	JPtrArray<Class>*	itsVisibleByName;			// doesn't own objects

	const TextFileType	itsFileType;
	JPtrArray<JString>*	itsSuffixList;
	JPtrArray<JString>*	itsCollapsedList;			// nullptr unless updating files
	bool				itsReparseAllFlag;			// true => flush all on next update
	bool				itsChangedDuringParseFlag;	// only used while parsing
	bool				itsBeganEmptyFlag;			// true => ignore RemoveFile()

	JSize		itsFontSize;
	JCoordinate	itsWidth;
	JCoordinate	itsHeight;
	const JSize	itsMarginWidth;
	bool		itsBroadcastClassSelFlag;

	bool	itsDrawMILinksOnTopFlag;

	bool	itsMinimizeMILinksFlag;
	bool	itsNeedsMinimizeMILinksFlag;		// true => links not currently minimized

	ClassStreamInFn	itsStreamInFn;

private:

	void	TreeX(TreeDirector* director, ClassStreamInFn streamInFn);

	void	RemoveFile(const JFAID_t id);

	void	RebuildTree();
	void	RecalcVisible(const bool forceRebuildVisible = false,
						  const bool forcePlaceAll = false);
	void	PlaceClass(Class* theClass, const JCoordinate x, JCoordinate* y,
					   JCoordinate* maxWidth);

	void	SaveCollapsedClasses(JPtrArray<JString>* list) const;
	bool	RestoreCollapsedClasses(const JPtrArray<JString>& list);

	void	MinimizeMILinks();
	bool	ArrangeRoots(const JArray<RootMIInfo>& rootList,
						 JArray<JIndex>* rootOrder, JProgressDisplay& pg) const;
	void	CleanList(JArray<RootSubset>* list) const;
	void	FindMIClasses(Class* theClass, JArray<bool>* marked,
						  const JArray<RootGeom>& rootGeom,
						  JArray<RootMIInfo>* rootList) const;
	void	FindRoots(Class* theClass, const JArray<RootGeom>& rootGeom,
					  JArray<RootMIInfo>* rootInfoList) const;
	bool	FindRoot(Class* root, const JArray<RootMIInfo>& rootInfoList,
					 JIndex* index) const;

	void	CollectAncestors(Class* cbClass, JPtrArray<Class>* list) const;

	static JListT::CompareResult
		CompareClassFullNames(Class* const & c1, Class* const & c2);
	static JListT::CompareResult
		CompareClassNames(Class* const & c1, Class* const & c2);

	static JListT::CompareResult
		CompareRGClassPtrs(const RootGeom& i1, const RootGeom& i2);
	static JListT::CompareResult
		CompareRSContent(const RootSubset& i1, const RootSubset& i2);

	// called by Class

	void	AddClass(Class* newClass);

	Class*	IndexToClassAfterRead(const JIndex index) const;
	JIndex	ClassToIndexForWrite(const Class* theClass) const;

	void	BroadcastSelectionChange(Class* theClass, const bool isSelected);

	// not allowed

	Tree(const Tree&) = delete;
	Tree& operator=(const Tree&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kChanged;
	static const JUtf8Byte* kBoundsChanged;
	static const JUtf8Byte* kNeedsRefresh;
	static const JUtf8Byte* kFontSizeChanged;

	static const JUtf8Byte* kPrepareForParse;
	static const JUtf8Byte* kParseFinished;

	static const JUtf8Byte* kClassSelected;
	static const JUtf8Byte* kClassDeselected;
	static const JUtf8Byte* kAllClassesDeselected;

	class Changed : public JBroadcaster::Message
	{
	public:

		Changed()
			:
			JBroadcaster::Message(kChanged)
			{ };
	};

	class BoundsChanged : public JBroadcaster::Message
	{
	public:

		BoundsChanged()
			:
			JBroadcaster::Message(kBoundsChanged)
			{ };
	};

	class NeedsRefresh : public JBroadcaster::Message
	{
	public:

		NeedsRefresh()
			:
			JBroadcaster::Message(kNeedsRefresh)
			{ };
	};

	class FontSizeChanged : public JBroadcaster::Message
	{
	public:

		FontSizeChanged(const JSize origSize, const JSize newSize,
						const JFloat vScaleFactor)
			:
			JBroadcaster::Message(kFontSizeChanged),
			itsOrigSize(origSize),
			itsNewSize(newSize),
			itsVertScaleFactor(vScaleFactor)
			{ };

		JSize
		GetOrigSize() const
		{
			return itsOrigSize;
		};

		JSize
		GetNewSize() const
		{
			return itsNewSize;
		};

		JFloat
		GetVertScaleFactor() const
		{
			return itsVertScaleFactor;
		};

	private:

		JSize	itsOrigSize, itsNewSize;
		JFloat	itsVertScaleFactor;
	};

	class PrepareForParse : public JBroadcaster::Message
	{
	public:

		PrepareForParse()
			:
			JBroadcaster::Message(kPrepareForParse)
			{ };
	};

	class ParseFinished : public JBroadcaster::Message
	{
	public:

		ParseFinished(const bool changed)
			:
			JBroadcaster::Message(kParseFinished),
			itsChangedFlag(changed)
			{ };

		bool
		Changed()
			const
		{
			return itsChangedFlag;
		};

	private:

		bool	itsChangedFlag;
	};

	class ClassSelected : public JBroadcaster::Message
	{
	public:

		ClassSelected(Class* theClass)
			:
			JBroadcaster::Message(kClassSelected),
			itsClass(theClass)
			{ };

		Class*
		GetClass()
			const
		{
			return itsClass;
		};

	private:

		Class*	itsClass;
	};

	class ClassDeselected : public JBroadcaster::Message
	{
	public:

		ClassDeselected(Class* theClass)
			:
			JBroadcaster::Message(kClassDeselected),
			itsClass(theClass)
			{ };

		Class*
		GetClass()
			const
		{
			return itsClass;
		};

	private:

		Class*	itsClass;
	};

	class AllClassesDeselected : public JBroadcaster::Message
	{
	public:

		AllClassesDeselected()
			:
			JBroadcaster::Message(kAllClassesDeselected)
			{ };
	};
};


/******************************************************************************
 GetClasses

	We do not guarantee any particular ordering of the classes.
	If nothing else, the list may not be sorted.

 ******************************************************************************/

inline const JPtrArray<Class>&
Tree::GetClasses()
	const
{
	return *itsClassesByFull;
}

inline JPtrArray<Class>*
Tree::GetClasses()
{
	return itsClassesByFull;
}

/******************************************************************************
 GetVisibleClasses

	We do not guarantee any particular ordering of the classes.
	If nothing else, the list may not be sorted.

 ******************************************************************************/

inline const JPtrArray<Class>&
Tree::GetVisibleClasses()
	const
{
	return *itsVisibleByName;
}

inline JPtrArray<Class>*
Tree::GetVisibleClasses()
{
	return itsVisibleByName;
}

/******************************************************************************
 GetTreeDirector

 ******************************************************************************/

inline TreeDirector*
Tree::GetTreeDirector()
	const
{
	return itsDirector;
}

/******************************************************************************
 GetFileType

 ******************************************************************************/

inline TextFileType
Tree::GetFileType()
	const
{
	return itsFileType;
}

/******************************************************************************
 GetFontSize

 ******************************************************************************/

inline JSize
Tree::GetFontSize()
	const
{
	return itsFontSize;
}

/******************************************************************************
 GetBounds

 ******************************************************************************/

inline void
Tree::GetBounds
	(
	JCoordinate* width,
	JCoordinate* height
	)
	const
{
	*width  = itsWidth;
	*height = itsHeight;
}

/******************************************************************************
 Show enums

 ******************************************************************************/

inline bool
Tree::WillAutoMinimizeMILinks()
	const
{
	return itsMinimizeMILinksFlag;
}

inline void
Tree::ShouldAutoMinimizeMILinks
	(
	const bool autoMinimize
	)
{
	if (autoMinimize != itsMinimizeMILinksFlag)
		{
		itsMinimizeMILinksFlag = autoMinimize;
		if (itsMinimizeMILinksFlag && itsNeedsMinimizeMILinksFlag)
			{
			RecalcVisible(true);
			}
		}
}

inline bool
Tree::NeedsMinimizeMILinks()
	const
{
	return itsNeedsMinimizeMILinksFlag;
}

/******************************************************************************
 Draw MI links on top of text

 ******************************************************************************/

inline bool
Tree::WillDrawMILinksOnTop()
	const
{
	return itsDrawMILinksOnTopFlag;
}

inline void
Tree::ShouldDrawMILinksOnTop
	(
	const bool drawOnTop
	)
{
	if (drawOnTop != itsDrawMILinksOnTopFlag)
		{
		itsDrawMILinksOnTopFlag = drawOnTop;
		Broadcast(NeedsRefresh());
		}
}

/******************************************************************************
 GetParseSuffixes

 ******************************************************************************/

inline const JPtrArray<JString>&
Tree::GetParseSuffixes()
	const
{
	return *itsSuffixList;
}

/******************************************************************************
 NextUpdateMustReparseAll

 ******************************************************************************/

inline void
Tree::NextUpdateMustReparseAll()
{
	itsReparseAllFlag = true;
}

/******************************************************************************
 NeedsReparseAll

 ******************************************************************************/

inline bool
Tree::NeedsReparseAll()
	const
{
	return itsReparseAllFlag;
}

/******************************************************************************
 RebuildLayout

 ******************************************************************************/

inline void
Tree::RebuildLayout()
{
	const JSize size = itsFontSize;
	itsFontSize      = 0;
	SetFontSize(size);
}

#endif
