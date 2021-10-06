/******************************************************************************
 DirList.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_DirList
#define _H_DirList

#include <jx-af/jcore/JContainer.h>
#include <jx-af/jcore/JPtrArray-JString.h>

struct DirInfo
{
	JString*	path;
	bool	recurse;
	JIndex		projIndex;	// for use by App::CollectSearchPaths()

	DirInfo()
		:
		path(nullptr), recurse(true), projIndex(0)
	{ };

	DirInfo(JString* p, const bool r)
		:
		path(p), recurse(r), projIndex(0)
	{ };

	static JListT::CompareResult
		ComparePathNames(const DirInfo& i1, const DirInfo& i2);
	static JListT::CompareResult
		CompareProjIndex(const DirInfo& i1, const DirInfo& i2);
};

class DirInfoList : public JArray<DirInfo>
{
public:

	void	DeleteAll();
};

class DirList : public JContainer
{
public:

	DirList();
	DirList(const DirList& source);

	virtual ~DirList();

	DirList& operator=(const DirList& source);

	bool	GetFullPath(const JIndex index, JString* fullPath,
							bool* recurse) const;
	bool	GetTruePath(const JIndex index, JString* truePath,
							bool* recurse) const;

	const JString&	GetPath(const JIndex index, bool* recurse) const;
	void			AddPath(const JString& path, const bool recurse);
	void			RemoveAll();

	const JString&	GetBasePath() const;
	void			SetBasePath(const JString& path);

	bool		Contains(const JString& path) const;

	void	ReadDirectories(std::istream& input, const JFileVersion vers);
	void	WriteDirectories(std::ostream& output) const;

private:

	DirInfoList*	itsDirList;
	JString			itsBasePath;

private:

	void	DirListX();
	void	ReadDirectory(std::istream& input, const JFileVersion vers);
	void	CopyPaths(const DirList& source);
};


// global functions for working with DirList

int operator==(const DirList& l1, const DirList& l2);


/******************************************************************************
 RemoveAll

 ******************************************************************************/

inline void
DirList::RemoveAll()
{
	itsDirList->DeleteAll();
}

/******************************************************************************
 GetBasePath

 ******************************************************************************/

inline const JString&
DirList::GetBasePath()
	const
{
	return itsBasePath;
}

/******************************************************************************
 operator!=

 ******************************************************************************/

inline int
operator!=
	(
	const DirList& l1,
	const DirList& l2
	)
{
	return !(l1 == l2);
}

#endif
