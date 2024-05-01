/******************************************************************************
 Class.cpp

	<Description>

	BASE CLASS = public JPtrArray<MemberFunction>

	Copyright (C) 2002 by Glenn W. Bach.
	
 *****************************************************************************/

#include "Class.h"
#include "Link.h"
#include "globals.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

Class::Class()
	:
	JPtrArray<MemberFunction>(JPtrArrayT::kDeleteAll),
	itsBaseClasses(nullptr),
	itsBaseClassFiles(nullptr),
	itsAncestors(nullptr),
	itsAncestorFiles(nullptr)
{
	itsLink	= jnew Link();
	ListenTo(itsLink);

	SetCompareFunction(MemberFunction::CompareFunction);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

Class::~Class()
{
	jdelete itsBaseClasses;
	jdelete itsBaseClassFiles;
	jdelete itsAncestors;
	jdelete itsAncestorFiles;
}

/******************************************************************************
 GetBaseClassCount (public)

 ******************************************************************************/

JSize
Class::GetBaseClassCount()
	const
{
	if (itsBaseClasses == nullptr)
	{
		return 0;
	}

	assert(itsBaseClasses->GetItemCount() == itsBaseClassFiles->GetItemCount());

	return itsBaseClasses->GetItemCount();
}

/******************************************************************************
 GetBaseClass (public)

 ******************************************************************************/

void
Class::GetBaseClass
	(
	const JIndex 	index,
	JString* 		classname, 
	JString* 		filename
	)
	const
{
	assert(itsBaseClasses != nullptr);
	assert(itsBaseClasses->IndexValid(index));
	assert(itsBaseClassFiles != nullptr);
	assert(itsBaseClassFiles->IndexValid(index));
	
	*classname	= *itsBaseClasses->GetItem(index);
	*filename	= *itsBaseClassFiles->GetItem(index);
}

/******************************************************************************
 AddBaseClass (public)

 ******************************************************************************/

void
Class::AddBaseClass
	(
	const JString& classname, 
	const JString& filename
	)
{
	if (itsBaseClasses == nullptr)
	{
		itsBaseClasses = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	}
	itsBaseClasses->Append(classname);

	if (itsBaseClassFiles == nullptr)
	{
		itsBaseClassFiles = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	}
	itsBaseClassFiles->Append(filename);
}

/******************************************************************************
 GetAncestorCount (public)

 ******************************************************************************/

JSize
Class::GetAncestorCount()
	const
{
	if (itsAncestors == nullptr)
	{
		return 0;
	}

	assert(itsAncestors->GetItemCount() == itsAncestorFiles->GetItemCount());

	return itsAncestors->GetItemCount();
}

/******************************************************************************
 GetAncestor (public)

 ******************************************************************************/

void
Class::GetAncestor
	(
	const JIndex 	index,
	JString* 		classname, 
	JString*		filename
	)
	const
{
	assert(itsAncestors != nullptr);
	assert(itsAncestors->IndexValid(index));
	assert(itsAncestorFiles != nullptr);
	assert(itsAncestorFiles->IndexValid(index));
	
	*classname	= *itsAncestors->GetItem(index);
	*filename	= *itsAncestorFiles->GetItem(index);
}

/******************************************************************************
 AddAncestor (public)

 ******************************************************************************/

void
Class::AddAncestor
	(
	const JString& classname, 
	const JString& filename
	)
{
	if (itsAncestors == nullptr)
	{
		itsAncestors = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	}
	itsAncestors->Append(classname);

	if (itsAncestorFiles == nullptr)
	{
		itsAncestorFiles = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	}
	itsAncestorFiles->Append(filename);
}

/******************************************************************************
 Populate (public)

 ******************************************************************************/

void
Class::Populate()
{
	const JSize bcount = GetBaseClassCount();
	if (bcount == 0)
	{
		return;
	}

	JString classname, filename;

	// Parse these in reverse order so newer declarations override
	// older ones.

	const JSize count = GetAncestorCount();
	for (JIndex i=count; i>=1; i--)
	{
		GetAncestor(i, &classname, &filename);
		itsLink->ParseClass(this, filename, classname);
	}

	// Parse these last so their function declarations override
	// those of their ancestors.

	for (JIndex i = 1; i <= bcount; i++)
	{
		GetBaseClass(i, &classname, &filename);
		itsLink->ParseClass(this, filename, classname);
	}
}

/******************************************************************************
 WritePublic (public)

 ******************************************************************************/

void
Class::WritePublic
	(
	std::ostream&	os,
	const bool		interface
	)
{
	for (auto* fn : *this)
	{
		if (fn->IsUsed() &&
			!fn->IsProtected())
		{
			WriteFunction(os, fn, interface);
		}
	}
}

/******************************************************************************
 WriteProtected (public)

 ******************************************************************************/

void
Class::WriteProtected
	(
	std::ostream&	os,
	const bool		interface
	)
{
	for (auto* fn : *this)
	{
		if (fn->IsUsed() &&
			fn->IsProtected())
		{
			WriteFunction(os, fn, interface);
		}
	}
}

/******************************************************************************
 WriteFunction (private)

 ******************************************************************************/

void
Class::WriteFunction
	(
	std::ostream& 	os,
	MemberFunction*	fn,
	const bool		interface
	)
{
	if (interface)
	{
		os << "\t";
		fn->GetReturnType().Print(os);
		os << ' ';
		fn->GetFnName().Print(os);
		fn->GetSignature().Print(os);
		os << " override;" << std::endl;
		return;
	}

	JString access("public");
	if (fn->IsProtected())
	{
		access = "protected";
	}

	JString s = GetPrefsManager()->GetFunctionComment(fn->GetFnName(), access);
	s.TrimWhitespace();
	s.Print(os);
	os << std::endl << std::endl;

	fn->GetReturnType().Print(os);
	os << std::endl;

	itsClassName.Print(os);
	os << "::";
	fn->GetFnName().Print(os);

	JString sig = fn->GetSignature();
	JStringIterator iter(&sig);
	if (!sig.StartsWith("()"))
	{
		// format arguments

		bool ok = iter.Next("(");
		assert( ok );
		iter.Insert("\n\t");

		iter.MoveTo(JStringIterator::kStartAtEnd, 0);
		ok = iter.Prev(")");
		assert( ok );
		iter.Insert("\n\t");

		ok = iter.Next(")");
		assert( ok );
		if (!iter.AtEnd())
		{
			JUtf8Character c;
			while (iter.Next(&c, JStringIterator::kStay) && c.IsSpace())
			{
				iter.RemoveNext();
			}
			iter.Insert("\n\t");
		}

		iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
		while (iter.Next(","))
		{
			iter.Insert("\n\t");
		}

		// print

		os << "\n\t";
	}

	iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
	const bool ok = iter.Next(")");
	assert( ok );
	if (!iter.AtEnd())
	{
		JUtf8Character c;
		while (iter.Next(&c, JStringIterator::kStay) && c.IsSpace())
		{
			iter.RemoveNext();
		}
		iter.Insert("\n\t");
	}

	sig.Print(os);
	os << "\n{\n\n}" << std::endl << std::endl;
}
