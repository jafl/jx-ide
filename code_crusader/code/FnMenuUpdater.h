/******************************************************************************
 FnMenuUpdater.h

	Copyright © 1999-2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_FnMenuUpdater
#define _H_FnMenuUpdater

#include <jx-af/jcore/JPrefObject.h>
#include "CtagsUser.h"

class JXTextMenu;

class FnMenuUpdater : public JPrefObject, public CtagsUser
{
public:

	FnMenuUpdater();

	virtual ~FnMenuUpdater();

	bool	CanCreateMenu(const TextFileType type) const;

	void		UpdateMenu(const JString& fileName, const TextFileType fileType,
						   const bool sort, const bool includeNS,
						   const bool pack,
						   JXTextMenu* menu, JArray<JIndex>* lineIndexList,
						   JArray<Language>* lineLangList);

	bool	WillSortFnNames() const;
	void		ShouldSortFnNames(const bool sort);

	bool	WillIncludeNamespace() const;
	void		ShouldIncludeNamespace(const bool includeNS);

	bool	WillPackFnNames() const;
	void		ShouldPackFnNames(const bool pack);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

private:

	bool	itsSortFlag;
	bool	itsIncludeNSFlag;
	bool	itsPackFlag;

private:

	void	ReadFunctionList(std::istream& input, const Language lang,
							 const bool sort, const bool includeNS,
							 JPtrArray<JString>* fnNameList,
							 JArray<JIndex>* lineIndexList,
							 JArray<Language>* lineLangList);

	// not allowed

	FnMenuUpdater(const FnMenuUpdater& source);
	const FnMenuUpdater& operator=(const FnMenuUpdater& source);
};


/******************************************************************************
 CanCreateMenu

 ******************************************************************************/

inline bool
FnMenuUpdater::CanCreateMenu
	(
	const TextFileType type
	)
	const
{
	return IsActive() && IsParsedForFunctionMenu(type);
}

/******************************************************************************
 Sorting

	This flag determines the default setting used by FunctionMenu.

 ******************************************************************************/

inline bool
FnMenuUpdater::WillSortFnNames()
	const
{
	return itsSortFlag;
}

inline void
FnMenuUpdater::ShouldSortFnNames
	(
	const bool sort
	)
{
	itsSortFlag = sort;
}

/******************************************************************************
 Namespace

	This flag determines the default setting used by FunctionMenu.

 ******************************************************************************/

inline bool
FnMenuUpdater::WillIncludeNamespace()
	const
{
	return itsIncludeNSFlag;
}

inline void
FnMenuUpdater::ShouldIncludeNamespace
	(
	const bool includeNS
	)
{
	itsIncludeNSFlag = includeNS;
}

/******************************************************************************
 Packing

	Use a smaller font size and pack the menu tightly.

 ******************************************************************************/

inline bool
FnMenuUpdater::WillPackFnNames()
	const
{
	return itsPackFlag;
}

inline void
FnMenuUpdater::ShouldPackFnNames
	(
	const bool pack
	)
{
	itsPackFlag = pack;
}

#endif
