/******************************************************************************
 SearchColorManager.cpp

	BASE CLASS = JColorManager

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "SearchColorManager.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SearchColorManager::SearchColorManager()
	:
	JColorManager()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SearchColorManager::~SearchColorManager()
{
}

/******************************************************************************
 GetColorID (virtual)

 ******************************************************************************/

bool
SearchColorManager::GetColorID
	(
	const JString&	name,
	JColorID*		id
	)
	const
{
	*id = 0;
	return true;
}
