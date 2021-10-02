/******************************************************************************
 Array2DCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_Array2DCommand
#define _H_Array2DCommand

#include "Command.h"
#include <jx-af/jcore/JPoint.h>
#include <jx-af/jcore/JRange.h>

class JStringTableData;
class JXStringTable;
class Array2DDir;

class Array2DCmd : public Command
{
public:

	enum UpdateType
	{
		kRow,
		kCol
	};

public:

	Array2DCmd(Array2DDir* dir,
					 JXStringTable* table, JStringTableData* data);

	virtual	~Array2DCmd();

	virtual void	PrepareToSend(const UpdateType type, const JIndex index,
								  const JInteger arrayIndex);

	Array2DDir*		GetDirector();
	JXStringTable*		GetTable();
	JStringTableData*	GetData();
	UpdateType			GetType() const;

protected:

	virtual void	HandleFailure();

	void	HandleFailure(const JIndex startIndex, const JString& value);
	JPoint	GetCell(const JIndex i) const;
	bool	ItsIndexValid() const;

private:

	Array2DDir*		itsDirector;
	JXStringTable*		itsTable;
	JStringTableData*	itsData;		// not owned
	UpdateType			itsType;
	JIndex				itsIndex;		// row or column to update
	JInteger			itsArrayIndex;	// i or j value being updated
};


/******************************************************************************
 GetDirector

 ******************************************************************************/

inline Array2DDir*
Array2DCmd::GetDirector()
{
	return itsDirector;
}

/******************************************************************************
 GetTable

 ******************************************************************************/

inline JXStringTable*
Array2DCmd::GetTable()
{
	return itsTable;
}

/******************************************************************************
 GetData

 ******************************************************************************/

inline JStringTableData*
Array2DCmd::GetData()
{
	return itsData;
}

/******************************************************************************
 GetType

 ******************************************************************************/

inline Array2DCmd::UpdateType
Array2DCmd::GetType()
	const
{
	return itsType;
}

#endif
