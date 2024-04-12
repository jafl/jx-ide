/******************************************************************************
 Class.cpp

	This class encapsulates everything the class browser needs to know about
	each class.

	Note that only Tree can delete Class.  Since extra generality is
	pointless in this case, it is too much trouble to allow others to
	delete us.  (Among other actions, one would have to call
	Tree::UpdatePlacement(), which is -very- expensive.)

	Derived classes must implement:

		ViewSource
			Open the appropriate file to display the implementation for
			this class.

		ViewHeader
			Open the appropriate file to display the declaration for this
			class.

		ViewDefinition
			Open the appropriate file to display the definition of the
			specified function.

		ViewDeclaration
			Open the appropriate file to display the declaration of the
			specified function.

		NewGhost
			Create a ghost of the appropriate derived type.

	Derived classes can override:

		StreamOut
			Write out extra data after calling Class::StreamOut().

		AdjustNameStyle
			Adjust the font style used to draw the class name.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1995-97 John Lindal.

 ******************************************************************************/

#include "Class.h"
#include "Tree.h"
#include "ProjectDocument.h"
#include "FileListTable.h"
#include "globals.h"
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// class data

JColorID Class::theGhostNameColor;

static JFontStyle kConcreteLabelStyle;
static JFontStyle kAbstractLabelStyle(false, true, 0, false, 0);

#ifndef CODE_CRUSADER_UNIT_TEST
const JCoordinate kHMarginWidth        = 3;
const JSize       kMinFrameWidth       = 50;
#endif
const JCoordinate kVMarginWidth        = 2;
const JCoordinate kLinkWidth           = 15;
const JCoordinate kCollapsedLinkLength = 14;

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

Class::Class
	(
	const JString&		fullName,
	const DeclareType	declType,
	const JFAID_t		fileID,
	Tree*				tree
	)
	:
	itsTree(tree),
	itsFullName(fullName),
	itsName(RemoveNamespace(fullName)),
	itsFileID(fileID)
{
	ClassX();

	itsDeclType       = declType;
	itsIsAbstractFlag = false;
	itsIsTemplateFlag = false;

	SetCoords(0,0);
	itsTree->AddClass(this);
}

Class::Class
	(
	std::istream&		input,
	const JFileVersion	vers,
	Tree*				tree
	)
	:
	itsTree(tree)
{
JIndex i;

	ClassX();

	if (vers < 30)
	{
		input >> i;
		assert( i == 1 );
	}

	input >> itsFullName;
	if (vers >= 6)
	{
		input >> itsName;
	}
	else
	{
		itsName = RemoveNamespace(itsFullName);
	}

	if (vers >= 1)
	{
		input >> itsDeclType;
	}
	else
	{
		itsDeclType = kClassType;
	}

	if (vers < 40)
	{
		JString fileName;
		input >> fileName;
		itsFileID = JFAID::kInvalidID;	// ignore ID since we will be tossed
	}
	else
	{
		input >> itsFileID;
	}

	input >> itsHCoord >> itsVCoord;

	if (vers >= 6)
	{
		input >> itsFrame;
	}
	else
	{
		CalcFrame();
	}

	if (vers == 0)
	{
		JIndex firstFoundParent;
		input >> firstFoundParent;
	}

	input >> JBoolFromString(itsIsAbstractFlag);

	itsIsTemplateFlag = false;
	if (vers >= 49)
	{
		input >> JBoolFromString(itsIsTemplateFlag);
	}

	if (vers >= 10)
	{
		input >> JBoolFromString(itsVisibleFlag)
			  >> JBoolFromString(itsCollapsedFlag)
			  >> JBoolFromString(itsIsSelectedFlag);
		if (vers == 10)
		{
			itsCollapsedFlag = false;	// itsWasVisibleFlag was true
		}
	}

	if (vers >= 11)
	{
		input >> JBoolFromString(itsHasPrimaryChildrenFlag)
			  >> JBoolFromString(itsHasSecondaryChildrenFlag);
	}

	// parents

	JSize parentCount;
	input >> parentCount;

	for (i=1; i<=parentCount; i++)
	{
		ParentInfo pInfo;
		pInfo.name = jnew JString;
		pInfo.ns   = jnew JString;

		input >> *pInfo.name;
		if (vers >= 97)
		{
			input >> *pInfo.ns;
		}
		input >> pInfo.indexFromFile >> pInfo.inheritance;
		itsParentInfo->AppendItem(pInfo);
	}

	// functions

	if (vers < 88)
	{
		JSize fnCount;
		input >> fnCount;

		JString name;
		JInteger access;
		bool pureVirtual;
		for (i=1; i<=fnCount; i++)
		{
			input >> name >> access >> JBoolFromString(pureVirtual);
		}
	}
}

// search target

Class::Class
	(
	const JString& name
	)
	:
	itsTree(nullptr),
	itsFullName(name),
	itsName(name),
	itsParentInfo(nullptr)
{
}

// private

void
Class::ClassX()
{
	itsParentInfo = jnew JArray<ParentInfo>;

	itsHasPrimaryChildrenFlag   = false;
	itsHasSecondaryChildrenFlag = false;

	itsVisibleFlag    = true;
	itsCollapsedFlag  = false;
	itsIsSelectedFlag = false;

	kConcreteLabelStyle       = JFontStyle();
	kAbstractLabelStyle.color = kConcreteLabelStyle.color;
}

/******************************************************************************
 Destructor

	Any or all can be nullptr if we were constructed as a search target.

 ******************************************************************************/

Class::~Class()
{
	if (itsParentInfo != nullptr)
	{
		for (auto pInfo : *itsParentInfo)
		{
			pInfo.CleanOut();
		}
		jdelete itsParentInfo;
	}
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
Class::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << ' ' << itsFullName << ' ' << itsName;
	output << ' ' << itsDeclType << ' ' << itsFileID;
	output << ' ' << itsHCoord << ' ' << itsVCoord << ' ' << itsFrame;
	output << ' ' << JBoolToString(itsIsAbstractFlag)
				  << JBoolToString(itsIsTemplateFlag)
				  << JBoolToString(itsVisibleFlag)
				  << JBoolToString(itsCollapsedFlag)
				  << JBoolToString(itsIsSelectedFlag)
				  << JBoolToString(itsHasPrimaryChildrenFlag)
				  << JBoolToString(itsHasSecondaryChildrenFlag);

	// parents

	const JSize parentCount = GetParentCount();
	output << ' ' << parentCount;

	for (JIndex i=1; i<=parentCount; i++)
	{
		const ParentInfo pInfo = itsParentInfo->GetItem(i);
		output << ' ' << *pInfo.name;
		output << ' ' << *pInfo.ns;
		output << ' ' << itsTree->ClassToIndexForWrite(pInfo.parent);
		output << ' ' << pInfo.inheritance;
	}

	output << ' ';
}

/******************************************************************************
 SetSelected

	Sets its state and tells itsTree to broadcast.

 ******************************************************************************/

void
Class::SetSelected
	(
	const bool selected
	)
{
	if (itsIsSelectedFlag != selected)
	{
		itsIsSelectedFlag = selected;
		itsTree->BroadcastSelectionChange(this, itsIsSelectedFlag);
	}
}

/******************************************************************************
 ForceVisible

	Uncollapses primary parents to make us visible.  If we have no parents,
	we simply show ourselves.

	*** Client is responsible for rebuilding tree afterwards.

 ******************************************************************************/

void
Class::ForceVisible()
{
	Class* c = this;
	Class* parent;
	while (c->GetParent(1, &parent) &&
		   (!parent->IsVisible() || parent->IsCollapsed()))
	{
		parent->SetCollapsed(false);
		c = parent;
	}
}

/******************************************************************************
 GetLanguage

	Returns the language of the class.

 ******************************************************************************/

Language
Class::GetLanguage()
	const
{
	return itsTree->GetLanguage();
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

bool
Class::GetFileName
	(
	JString* fileName
	)
	const
{
	if (itsFileID != JFAID::kInvalidID)
	{
		*fileName = itsTree->GetProjectDoc()->GetAllFileList()->
					GetFileName(itsFileID);
		return true;
	}
	else
	{
		fileName->Clear();
		return false;		// ghost
	}
}

#endif

/******************************************************************************
 AddParent

 ******************************************************************************/

void
Class::AddParent
	(
	const InheritType	type,
	const JString&		name,
	const JString&		ns
	)
{
	ParentInfo pInfo(jnew JString(name), jnew JString(ns), nullptr, type);
	assert( pInfo.name != nullptr );
	itsParentInfo->AppendItem(pInfo);
}

/******************************************************************************
 ClearConnections (private)

	Called by Tree before FindParents() iteration.

 ******************************************************************************/

void
Class::ClearConnections()
{
	const JSize parentCount = GetParentCount();
	for (JIndex i=1; i<=parentCount; i++)
	{
		ParentInfo pInfo = itsParentInfo->GetItem(i);
		pInfo.parent     = nullptr;
		itsParentInfo->SetItem(i, pInfo);
	}

	itsHasPrimaryChildrenFlag   = false;
	itsHasSecondaryChildrenFlag = false;
}

/******************************************************************************
 FindParents

	Search the class tree for our parents and update itsParentInfo.

	Returns true if it managed to fill in another nullptr parent.

 ******************************************************************************/

bool
Class::FindParents
	(
	const bool okToCreateGhost
	)
{
	bool foundAnotherParent = false;

	const JSize parentCount = GetParentCount();
	for (JIndex i=1; i<=parentCount; i++)
	{
		ParentInfo pInfo         = itsParentInfo->GetItem(i);
		const bool parentWasNull = pInfo.parent == nullptr;

		if (parentWasNull &&
			!FindParent(&pInfo, okToCreateGhost) && okToCreateGhost)
		{
			pInfo.parent = NewGhost(*pInfo.name, itsTree);
		}

		if (parentWasNull && pInfo.parent != nullptr)
		{
			foundAnotherParent = true;
			pInfo.parent->AddChild(this, i==1);
			itsParentInfo->SetItem(i, pInfo);
		}
	}

	return foundAnotherParent;
}

/******************************************************************************
 FindParent (private)

	Search the tree for the given parent.  First, we check if we find
	an exact match.  Otherwise, we have to work our way up our namespace
	to see if any of the classes that contain us (or a parent thereof)
	contains our parent.

	Example: JTextEditor::TextChanged inherits from JBroadcaster::Message.

 ******************************************************************************/

bool
Class::FindParent
	(
	ParentInfo*	pInfo,
	const bool	okToSearchGhosts
	)
{
	const JUtf8Byte* namespaceOp = GetNamespaceOperator(itsTree->GetLanguage());

	// check for exact match that isn't a ghost

	JString nameSpace = *pInfo->ns, testName;
	while (true)
	{
		testName = *pInfo->name;
		if (!nameSpace.IsEmpty())
		{
			testName.Prepend(namespaceOp);
			testName.Prepend(nameSpace);
		}

		if (itsTree->FindClass(testName, &pInfo->parent) &&
			pInfo->parent != this &&
			!pInfo->parent->IsGhost())
		{
			return true;
		}

		if (nameSpace.IsEmpty())
		{
			break;
		}

		JStringIterator iter(&nameSpace, JStringIterator::kStartAtEnd);
		if (iter.Prev(namespaceOp))
		{
			iter.RemoveAllNext();
		}
		else
		{
			nameSpace.Clear();
		}
	}

	// try all possible namespaces to look for existing parent

	nameSpace = itsFullName;
	JString prefixStr;

	JStringIterator iter(&nameSpace, JStringIterator::kStartAtEnd);
	while (iter.Prev(namespaceOp) && !iter.AtBeginning())
	{
		iter.RemoveAllNext();

		// check if parent exists in namespace

		testName  = nameSpace;
		testName += namespaceOp;
		testName += *pInfo->name;
		if (itsTree->FindClass(testName, &pInfo->parent) &&
			pInfo->parent != this)
		{
			*pInfo->name = testName;
			return true;
		}

		// if namespace exists as a class, check its ancestors

		const Class* nsClass;
		if (itsTree->FindClass(nameSpace, &nsClass) &&
			itsTree->FindParent(*pInfo->name, nsClass, &pInfo->parent, &prefixStr) &&
			pInfo->parent != this)
		{
			pInfo->name->Prepend(prefixStr);
			return true;
		}
	}

	// check for any exact match

	return okToSearchGhosts &&
				itsTree->FindClass(*pInfo->name, &pInfo->parent) &&
				pInfo->parent != this;
}

/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost using the appropriate derived class.  The file name
	is not passed in because it can be empty.

 ******************************************************************************/

Class*
Class::NewGhost
	(
	const JString&	name,
	Tree*			tree
	)
{
	assert_msg( 0, "The programmer forgot to override Class::NewGhost()" );
	return nullptr;
}

/******************************************************************************
 RemoveNamespace (protected)

	Extracts the name of the class without the namespace prefix.

 ******************************************************************************/

JString
Class::RemoveNamespace
	(
	const JString& fullName
	)
{
	JString name = fullName;

	const JUtf8Byte* nso = GetNamespaceOperator(itsTree->GetLanguage());
	JStringIterator iter(&name, JStringIterator::kStartAtEnd);
	if (iter.Prev(nso))
	{
		iter.Next(nso);
		iter.RemoveAllPrev();
	}

	return name;
}

/******************************************************************************
 GetParent

	This function will return false if the parent is not part of the tree.

 ******************************************************************************/

bool
Class::GetParent
	(
	const JIndex	index,
	const Class**	parent
	)
	const
{
	return GetParent(index, const_cast<Class**>(parent));
}

bool
Class::GetParent
	(
	const JIndex	index,
	Class**		parent
	)
	const
{
	if (itsParentInfo->IndexValid(index))
	{
		ParentInfo pInfo = itsParentInfo->GetItem(index);
		*parent          = pInfo.parent;
		return *parent != nullptr;
	}
	else
	{
		*parent = nullptr;
		return false;
	}
}

/******************************************************************************
 IsAncestor

	Returns true if we are an ancestor of the given class.

	We are an ancestor of child if we are one of its parents
	or an ancestor of one of its parents.

 ******************************************************************************/

bool
Class::IsAncestor
	(
	const Class* child
	)
	const
{
	const JIndex parentCount = child->GetParentCount();
	for (JIndex i=1; i<=parentCount; i++)
	{
		const Class* p;
		if (child->GetParent(i, &p) && (this == p || IsAncestor(p)))
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 GetAncestorList (virtual)

	Java must use name instead of full name.

 ******************************************************************************/

void
Class::GetAncestorList
	(
	JPtrArray<JString>* list
	)
	const
{
	for (const auto* s : *list)
	{
		if (*s == itsFullName)
		{
			return;
		}
	}

	list->Append(itsFullName);

	for (const auto& info : *itsParentInfo)
	{
		if (info.parent != nullptr)
		{
			info.parent->GetAncestorList(list);
		}
	}
}

/******************************************************************************
 AddChild (private)

	We sort the function names so others can perform an incremental search.

 ******************************************************************************/

void
Class::AddChild
	(
	Class*		child,
	const bool	primary
	)
{
	if (primary)
	{
		itsHasPrimaryChildrenFlag = true;
	}
	else
	{
		itsHasSecondaryChildrenFlag = true;
	}
}

/******************************************************************************
 View... (virtual)

	These are not pure virtual because Tree needs to be able to construct
	a Class as a search target.

 ******************************************************************************/

void
Class::ViewSource()
	const
{
	assert_msg( 0, "The programmer forgot to override Class::ViewSource()" );
}

void
Class::ViewHeader()
	const
{
	assert_msg( 0, "The programmer forgot to override Class::ViewHeader()" );
}

/******************************************************************************
 FindParentsAfterRead

	Called by Tree after reading in classes.

 ******************************************************************************/

void
Class::FindParentsAfterRead()
{
	const JSize count = itsParentInfo->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		ParentInfo pInfo    = itsParentInfo->GetItem(i);
		pInfo.parent        = itsTree->IndexToClassAfterRead(pInfo.indexFromFile);
		pInfo.indexFromFile = 0;	// make sure we never use it again
		itsParentInfo->SetItem(i, pInfo);
	}
}

/******************************************************************************
 NeedToDrawLink (private)

 ******************************************************************************/

inline bool
Class::NeedToDrawLink
	(
	const JPoint&	pt1,
	const JPoint&	pt2,
	const JRect&	visRect
	)
	const
{
	return !((pt1.y < visRect.top    && pt2.y < visRect.top) ||
			 (pt1.y > visRect.bottom && pt2.y > visRect.bottom));
}

/******************************************************************************
 Link points (private)

 ******************************************************************************/

inline JPoint
Class::GetLinkFromPt()
	const
{
	return JPoint(itsFrame.right, itsFrame.ycenter());
}

inline JPoint
Class::GetLinkToPt()
	const
{
	return JPoint(itsFrame.left, itsFrame.ycenter());
}

/******************************************************************************
 NeedDrawName (private)

	Most classes don't need GetDrawName(), and Draw() goes faster if
	we don't have to copy strings.

 ******************************************************************************/

inline bool
Class::NeedDrawName()
	const
{
	return itsIsTemplateFlag;
}

/******************************************************************************
 GetDrawName (private)

 ******************************************************************************/

JString
Class::GetDrawName()
	const
{
	JString drawName = itsFullName;
	if (itsIsTemplateFlag)
	{
		drawName.Append(JGetString("TemplateNameSuffix::Class"));
	}

	return drawName;
}

/******************************************************************************
 Draw

 ******************************************************************************/

void
Class::Draw
	(
	JPainter&		p,
	const JRect&	rect
	)
	const
{
	// only draw box if visible

	JRect irect;
	if (JIntersection(itsFrame, rect, &irect))
	{
		if (itsIsSelectedFlag && itsDeclType == kGhostType)
		{
			p.SetPenColor(JColorManager::GetBlueColor());
		}
		else if (itsIsSelectedFlag)
		{
			p.SetPenColor(JColorManager::GetCyanColor());
		}
		else if (itsDeclType == kGhostType)
		{
			p.SetPenColor(JColorManager::GetGrayColor(80));
		}
		else
		{
			p.SetPenColor(JColorManager::GetWhiteColor());
		}
		p.SetFilling(true);
		p.Rect(itsFrame);

		// draw black frame

		p.SetFilling(false);
		p.SetPenColor(JColorManager::GetBlackColor());
		p.Rect(itsFrame);
	}

	// draw dashed line if collapsed and has children

	if (itsCollapsedFlag && itsHasPrimaryChildrenFlag)
	{
		const JPoint pt = GetLinkFromPt();
		if (rect.top <= pt.y && pt.y <= rect.bottom)
		{
			// remember to update kCollapsedLinkLength

			p.SetPenColor(JColorManager::GetBlackColor());
			p.Line(pt.x   ,pt.y, pt.x+2 ,pt.y);
			p.Line(pt.x+6 ,pt.y, pt.x+8 ,pt.y);
			p.Line(pt.x+12,pt.y, pt.x+14,pt.y);
		}
	}

	// primary inheritance

	Class* parent;
	if (GetParent(1, &parent))
	{
		const JPoint linkToPt   = GetLinkToPt();
		const JPoint linkFromPt = parent->GetLinkFromPt();
		if (NeedToDrawLink(linkFromPt, linkToPt, rect))
		{
			const InheritType type = GetParentType(1);
			if (type == kInheritPublic)
			{
				p.SetPenColor(JColorManager::GetBlackColor());
			}
			else if (type == kInheritProtected)
			{
				p.SetPenColor(JColorManager::GetYellowColor());
			}
			else
			{
				assert( type == kInheritPrivate );
				p.SetPenColor(JColorManager::GetRedColor());
			}

			const JCoordinate x = (linkFromPt.x + linkToPt.x)/2;
			p.Line(linkToPt.x, linkToPt.y, x, linkToPt.y);
			p.SetPenColor(JColorManager::GetBlackColor());
			p.LineTo(x, linkFromPt.y);
			p.LineTo(linkFromPt);
		}
	}
}

/******************************************************************************
 DrawMILinks

	We draw the secondary inheritance links separately so they can be
	drawn either above or below everything else.

 ******************************************************************************/

void
Class::DrawMILinks
	(
	JPainter&		p,
	const JRect&	rect
	)
	const
{
	const JSize parentCount = GetParentCount();
	if (parentCount < 2)
	{
		return;
	}

	const JPoint linkToPt = GetLinkToPt();

	for (JIndex i=2; i<=parentCount; i++)
	{
		Class* parent;
		const bool ok = GetParent(i, &parent);
		assert( ok );

		JCoordinate deltaLinkFromX = 0;
		while (!parent->IsVisible())
		{
			deltaLinkFromX    = kCollapsedLinkLength;
			const bool ok = parent->GetParent(1, &parent);
			assert( ok );
		}

		const JPoint linkFromPt = parent->GetLinkFromPt() + JPoint(deltaLinkFromX,0);
		if (NeedToDrawLink(linkFromPt, linkToPt, rect))
		{
			const InheritType type = GetParentType(i);
			if (type == kInheritPublic)
			{
				p.SetPenColor(JColorManager::GetGreenColor());
			}
			else if (type == kInheritProtected)
			{
				p.SetPenColor(JColorManager::GetYellowColor());
			}
			else
			{
				assert( type == kInheritPrivate );
				p.SetPenColor(JColorManager::GetRedColor());
			}

			p.Line(linkFromPt, linkToPt);
		}
	}
}

/******************************************************************************
 DrawText

	We draw the text separately so it can be drawn above the MI links.

 ******************************************************************************/

void
Class::DrawText
	(
	JPainter&		p,
	const JRect&	rect
	)
	const
{
	// only draw text if visible

	JRect irect;
	if (JIntersection(itsFrame, rect, &irect))
	{
		JFontStyle style = kConcreteLabelStyle;
		AdjustNameStyle(&style);
		p.SetFontStyle(style);

		if (NeedDrawName())
		{
			const JString drawName = GetDrawName();
			p.String(itsFrame, drawName,
					 JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
		}
		else
		{
			p.String(itsFrame, itsFullName,
					 JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
		}
	}
}

/******************************************************************************
 AdjustNameStyle (virtual protected)

 ******************************************************************************/

void
Class::AdjustNameStyle
	(
	JFontStyle* style
	)
	const
{
	if (itsIsAbstractFlag)
	{
		*style = kAbstractLabelStyle;
	}

	if (itsIsSelectedFlag && itsDeclType == kGhostType)
	{
		style->color = JColorManager::GetWhiteColor();
	}
	else if (itsDeclType == kGhostType)
	{
		style->color = GetGhostNameColor();
	}
}

/******************************************************************************
 CalcFrameHeight (static private)

 ******************************************************************************/

inline JCoordinate
Class::CalcFrameHeight
	(
	JFontManager*	fontManager,
	const JSize		fontSize
	)
{
	JFont font = JFontManager::GetDefaultFont();
	font.SetSize(fontSize);
	font.SetStyle(kConcreteLabelStyle);
	return 2*kVMarginWidth + font.GetLineHeight(fontManager);
}

/******************************************************************************
 GetTotalWidth

	Return the width (in pixels) that this class takes up in the tree.

 ******************************************************************************/

JCoordinate
Class::GetTotalWidth()
	const
{
	const JCoordinate w = itsFrame.width();
	return (HasParents() ? w + kLinkWidth : w);
}

/******************************************************************************
 GetTotalHeight

	Return the height (in pixels) that this class takes up in the tree.

 ******************************************************************************/

JCoordinate
Class::GetTotalHeight()
	const
{
	return 2*kVMarginWidth + itsFrame.height();
}

// static

JCoordinate
Class::GetTotalHeight
	(
	Tree*			tree,
	JFontManager*	fontManager
	)
{
	return 2*kVMarginWidth + CalcFrameHeight(fontManager, tree->GetFontSize());
}

/******************************************************************************
 CalcFrame

 ******************************************************************************/

void
Class::CalcFrame()
{
#ifndef CODE_CRUSADER_UNIT_TEST

	JCoordinate x = itsHCoord;
	if (HasParents())
	{
		x += kLinkWidth;
	}

	JFontManager* fontManager = GetTree()->GetFontManager();
	const JSize fontSize      = itsTree->GetFontSize();

	const JString name = GetDrawName();

	JFontStyle style = kConcreteLabelStyle;
	style.bold       = true;	// if not bold, extra space doesn't hurt

	JFont font = JFontManager::GetDefaultFont();
	font.SetSize(fontSize);
	font.SetStyle(style);

	const JCoordinate w =
		JMax(kMinFrameWidth, 2*kHMarginWidth + font.GetStringWidth(fontManager, name));

	const JCoordinate h = CalcFrameHeight(fontManager, fontSize);

	itsFrame.Set(itsVCoord, x, itsVCoord+h, x+w);

#endif
}

/******************************************************************************
 Class::ParentInfo

 ******************************************************************************/

void
Class::ParentInfo::CleanOut()
{
	jdelete name;
	name = nullptr;

	jdelete ns;
	ns = nullptr;
}

/******************************************************************************
 Global functions for Class class

 ******************************************************************************/

/******************************************************************************
 Global functions for Class::DeclareType

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&			input,
	Class::DeclareType&	type
	)
{
	long temp;
	input >> temp;
	type = (Class::DeclareType) temp;
	assert( type == Class::kClassType ||
			type == Class::kStructUnused ||
			type == Class::kGhostType ||
			type == Class::kEnumUnused );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&				output,
	const Class::DeclareType	type
	)
{
	output << (long) type;
	return output;
}

/******************************************************************************
 Global functions for Class::InheritType

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&			input,
	Class::InheritType&	type
	)
{
	long temp;
	input >> temp;
	type = (Class::InheritType) temp;
	assert( type == Class::kInheritPublic    ||
			type == Class::kInheritProtected ||
			type == Class::kInheritPrivate   ||
			type == Class::kInheritJavaDefault);
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&				output,
	const Class::InheritType	type
	)
{
	output << (long) type;
	return output;
}
