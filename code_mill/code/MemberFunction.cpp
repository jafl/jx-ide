/******************************************************************************
 MemberFunction.cpp

	BASE CLASS = none

	Copyright (C) 2002 by Glenn W. Bach.
	
 *****************************************************************************/

#include <MemberFunction.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

MemberFunction::MemberFunction()
	:	
	itsArgs(nullptr)
{
	itsIsProtected	= false;
	itsIsRequired	= false;
	itsIsUsed		= false;
	itsIsConst		= false;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MemberFunction::~MemberFunction()
{
	jdelete itsArgs;
}

/******************************************************************************
 SetInterface (public)

 ******************************************************************************/

static const JRegex pureVirtualPattern = "[[:space:]]*=[[:space:]]*0;$";

void
MemberFunction::SetInterface
	(
	const JString& interface
	)
{
	itsInterface = interface;

	JStringIterator iter(&itsInterface);
	if (iter.Next(pureVirtualPattern))
	{
		iter.ReplaceLastMatch(";");
	}
}

/******************************************************************************
 GetArg (public)

 ******************************************************************************/

const JString&
MemberFunction::GetArg
	(
	const JIndex index
	)
	const
{
	assert(itsArgs != nullptr);
	assert(itsArgs->IndexValid(index));
	return *(itsArgs->GetElement(index));
}

/******************************************************************************
 AddArg (public)

 ******************************************************************************/

void
MemberFunction::AddArg
	(
	const JString& arg
	)
{
	if (itsArgs == nullptr)
	{
		itsArgs = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert(itsArgs != nullptr);
	}

	itsArgs->Append(arg);

	if (!itsArgString.IsEmpty())
	{
		itsArgString += ", ";
	}
	itsArgString += arg;
}

/******************************************************************************
 CompareFunction (static private)

 ******************************************************************************/

JListT::CompareResult
MemberFunction::CompareFunction
	(
	MemberFunction* const & f1,
	MemberFunction* const & f2
	)
{
	const JString& name1 = f1->GetFnName();
	const JString& name2 = f2->GetFnName();
	JListT::CompareResult r =
		JCompareStringsCaseInsensitive(
			const_cast<JString*>(&name1), const_cast<JString*>(&name2));
	if (r != JListT::kFirstEqualSecond)
	{
		return r;
	}

	if (f1->IsConst() != f2->IsConst())
	{
		if (f1->IsConst())
		{
			return JListT::kFirstLessSecond;
		}
		return JListT::kFirstGreaterSecond;
	}

	const JString& type1 = f1->GetReturnType();
	const JString& type2 = f2->GetReturnType();
	r = JCompareStringsCaseInsensitive(const_cast<JString*>(&type1),
									   const_cast<JString*>(&type2));
	if (r != JListT::kFirstEqualSecond)
	{
		return r;
	}

	return JCompareStringsCaseInsensitive(const_cast<JString*>(&(f1->GetArgString())),
										  const_cast<JString*>(&(f2->GetArgString())));
}
