/******************************************************************************
 Class.h

	Interface for the Class Class

	Copyright © 1995-97 John Lindal.

 ******************************************************************************/

#ifndef _H_Class
#define _H_Class

#include "TextFileType.h"		// need defn of Language
#include <jx-af/jcore/JBroadcaster.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JFontStyle.h>
#include <jx-af/jcore/JRect.h>
#include <jx-af/jcore/JFAID.h>
#include <jx-af/jcore/jColor.h>

class JPainter;
class JFontManager;
class Tree;

class Class : virtual public JBroadcaster
{
	friend class Tree;

public:

	// Do not change these values once they are assigned
	// because they are stored in files.

	enum DeclareType
	{
		kClassType = 0,
		kStructUnused,
		kGhostType,		// inferred, but not parsed
		kEnumUnused
	};

	enum InheritType
	{
		kInheritPublic = 0,
		kInheritProtected,
		kInheritPrivate,
		kInheritJavaDefault
	};

public:

	~Class() override;

	const Tree*	GetTree() const;

	const JString&	GetFullName() const;
	const JString&	GetName() const;
	bool			GetFileID(JFAID_t* id) const;
	bool			GetFileName(JString* fileName) const;

	virtual void	ViewSource() const;
	virtual void	ViewHeader() const;

	Language	GetLanguage() const;
	DeclareType	GetDeclareType() const;
	bool		IsAbstract() const;
	void		SetAbstract(const bool abstract = true);
	bool		IsTemplate() const;
	void		SetTemplate(const bool tmpl = true);
	bool		IsGhost() const;

	void			AddParent(const InheritType type, const JString& name);
	bool			FindParents(const bool okToCreateGhost);
	bool			HasParents() const;
	JSize			GetParentCount() const;
	const JString&	GetParentName(const JIndex index) const;
	InheritType		GetParentType(const JIndex index) const;
	bool			GetParent(const JIndex index, Class** parent) const;
	bool			GetParent(const JIndex index, const Class** parent) const;
	bool			IsAncestor(const Class* child) const;
	void			GetAncestorList(JPtrArray<JString>* fullNameList) const;

	bool	HasChildren() const;
	bool	HasPrimaryChildren() const;
	bool	HasSecondaryChildren() const;

	void	Draw(JPainter& p, const JRect& rect) const;
	void	DrawMILinks(JPainter& p, const JRect& rect) const;
	void	DrawText(JPainter& p, const JRect& rect) const;

	const JRect&		GetFrame() const;
	bool				Contains(const JPoint& pt) const;
	JCoordinate			GetTotalWidth() const;
	JCoordinate			GetTotalHeight() const;
	static JCoordinate	GetTotalHeight(Tree* tree, JFontManager* fontManager);
	void				GetCoords(JCoordinate* x, JCoordinate* y) const;
	void				SetCoords(const JCoordinate x, const JCoordinate y);

	bool	IsVisible() const;
	void	SetVisible(const bool visible);
	void	ForceVisible();

	bool	IsSelected() const;
	void	SetSelected(const bool selected);
	void	ToggleSelected();

	bool	IsCollapsed() const;
	void	SetCollapsed(const bool collapse);

	virtual void	StreamOut(std::ostream& output) const;

	static JColorID	GetGhostNameColor();
	static void		SetGhostNameColor(const JColorID color);

	// called by Tree::ReloadSetup

	void	CalcFrame();

protected:

	Class(const JString& fullName, const DeclareType declType,
		  const JFAID_t fileID, Tree* tree);
	Class(std::istream& input, const JFileVersion vers, Tree* tree);
	Class(const JString& name);	// search target

	virtual Class*	NewGhost(const JString& name, Tree* tree);
	JString			RemoveNamespace(const JString& fullName);

	virtual void	AdjustNameStyle(JFontStyle* style) const;

private:

	struct ParentInfo
	{
		JString*	name;
		Class*		parent;			// can be nullptr; not owned
		InheritType	inheritance;
		JIndex		indexFromFile;	// used while reading from file

		ParentInfo()
			:
			name(nullptr),
			parent(nullptr),
			inheritance(kInheritPublic),
			indexFromFile(0)
			{ };

		ParentInfo(JString* n, Class* p, const InheritType type)
			:
			name(n),
			parent(p),
			inheritance(type),
			indexFromFile(0)
			{ };

		void CleanOut();
	};

private:

	Tree*		itsTree;				// our owner; first to allow constructing itsName
	JString		itsFullName;			// namespace + class name
	JString		itsName;				// class name
	DeclareType	itsDeclType;
	JFAID_t		itsFileID;
	bool		itsIsAbstractFlag;		// true if contains pure virtual functions
	bool		itsIsTemplateFlag;

	JCoordinate	itsHCoord;				// coordinates in class tree
	JCoordinate	itsVCoord;
	JRect		itsFrame;

	bool	itsVisibleFlag;
	bool	itsCollapsedFlag;			// true => children are invisible
	bool	itsIsSelectedFlag;

	JArray<ParentInfo>*	itsParentInfo;

	bool	itsHasPrimaryChildrenFlag;
	bool	itsHasSecondaryChildrenFlag;

	static JColorID	theGhostNameColor;

private:

	void	ClassX();
	bool	FindParent(ParentInfo* pInfo, const bool okToSearchGhosts);
	void	AddChild(Class* child, const bool primary);

	static JCoordinate	CalcFrameHeight(JFontManager* fontManager,
										const JSize fontSize);

	bool	NeedDrawName() const;
	JString	GetDrawName() const;

	bool	NeedToDrawLink(const JPoint& pt1, const JPoint& pt2,
						   const JRect& visRect) const;
	JPoint	GetLinkFromPt() const;
	JPoint	GetLinkToPt() const;

	// called by Tree

	void	ClearConnections();
	void	FindParentsAfterRead();

	// not allowed

	Class(const Class&) = delete;
	Class& operator=(const Class&) = delete;
};

std::istream& operator>>(std::istream& input, Class::DeclareType& type);
std::ostream& operator<<(std::ostream& output, const Class::DeclareType type);

std::istream& operator>>(std::istream& input, Class::InheritType& type);
std::ostream& operator<<(std::ostream& output, const Class::InheritType type);


/******************************************************************************
 GetFullName

	Returns the name of the class including the namespace prefix.

 ******************************************************************************/

inline const JString&
Class::GetFullName()
	const
{
	return itsFullName;
}

/******************************************************************************
 GetName

	Returns the name of the class without the namespace prefix.

 ******************************************************************************/

inline const JString&
Class::GetName()
	const
{
	return itsName;
}

/******************************************************************************
 GetDeclareType (protected)

 ******************************************************************************/

inline Class::DeclareType
Class::GetDeclareType()
	const
{
	return itsDeclType;
}

inline bool
Class::IsAbstract()
	const
{
	return itsIsAbstractFlag;
}

inline bool
Class::IsGhost()
	const
{
	return itsDeclType == kGhostType;
}

/******************************************************************************
 SetAbstract

 ******************************************************************************/

inline void
Class::SetAbstract
	(
	const bool abstract
	)
{
	itsIsAbstractFlag = abstract;
}

/******************************************************************************
 Template

 ******************************************************************************/

inline bool
Class::IsTemplate()
	const
{
	return itsIsTemplateFlag;
}

inline void
Class::SetTemplate
	(
	const bool tmpl
	)
{
	itsIsTemplateFlag = tmpl;
	CalcFrame();
}

/******************************************************************************
 GetFileID

 ******************************************************************************/

inline bool
Class::GetFileID
	(
	JFAID_t* id
	)
	const
{
	*id = itsFileID;
	return itsFileID != JFAID::kInvalidID;
}

/******************************************************************************
 HasParents

 ******************************************************************************/

inline bool
Class::HasParents()
	const
{
	return !itsParentInfo->IsEmpty();
}

/******************************************************************************
 GetParentCount

 ******************************************************************************/

inline JSize
Class::GetParentCount()
	const
{
	return itsParentInfo->GetElementCount();
}

/******************************************************************************
 GetParentName

 ******************************************************************************/

inline const JString&
Class::GetParentName
	(
	const JIndex index
	)
	const
{
	return *((itsParentInfo->GetElement(index)).name);
}

/******************************************************************************
 GetParentType

 ******************************************************************************/

inline Class::InheritType
Class::GetParentType
	(
	const JIndex index
	)
	const
{
	return (itsParentInfo->GetElement(index)).inheritance;
}

/******************************************************************************
 Children

 ******************************************************************************/

inline bool
Class::HasChildren()
	const
{
	return itsHasPrimaryChildrenFlag || itsHasSecondaryChildrenFlag;
}

inline bool
Class::HasPrimaryChildren()
	const
{
	return itsHasPrimaryChildrenFlag;
}

inline bool
Class::HasSecondaryChildren()
	const
{
	return itsHasSecondaryChildrenFlag;
}

/******************************************************************************
 GetCoords

 ******************************************************************************/

inline void
Class::GetCoords
	(
	JCoordinate* x,
	JCoordinate* y
	)
	const
{
	*x = itsHCoord;
	*y = itsVCoord;
}

/******************************************************************************
 SetCoords

 ******************************************************************************/

inline void
Class::SetCoords
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	itsHCoord = x;
	itsVCoord = y;
	CalcFrame();
}

/******************************************************************************
 GetFrame

 ******************************************************************************/

inline const JRect&
Class::GetFrame()
	const
{
	return itsFrame;
}

/******************************************************************************
 Contains

 ******************************************************************************/

inline bool
Class::Contains
	(
	const JPoint& pt
	)
	const
{
	return itsFrame.Contains(pt);
}

/******************************************************************************
 Visible

 ******************************************************************************/

inline bool
Class::IsVisible()
	const
{
	return itsVisibleFlag;
}

inline void
Class::SetVisible
	(
	const bool visible
	)
{
	itsVisibleFlag = visible;
}

/******************************************************************************
 Selection

 ******************************************************************************/

inline bool
Class::IsSelected()
	const
{
	return itsIsSelectedFlag;
}

inline void
Class::ToggleSelected()
{
	SetSelected(!itsIsSelectedFlag);
}

/******************************************************************************
 Collapsed

 ******************************************************************************/

inline bool
Class::IsCollapsed()
	const
{
	return itsCollapsedFlag;
}

inline void
Class::SetCollapsed
	(
	const bool collapse
	)
{
	itsCollapsedFlag = collapse;
}

/******************************************************************************
 GetTree

 ******************************************************************************/

inline const Tree*
Class::GetTree()
	const
{
	return itsTree;
}

/******************************************************************************
 Ghost name color (static)

 ******************************************************************************/

inline JColorID
Class::GetGhostNameColor()
{
	return theGhostNameColor;
}

inline void
Class::SetGhostNameColor
	(
	const JColorID color
	)
{
	theGhostNameColor = color;
}

#endif
