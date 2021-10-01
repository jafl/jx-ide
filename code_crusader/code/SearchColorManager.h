/******************************************************************************
 SearchColorManager.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_SearchColorManager
#define _H_SearchColorManager

#include <jx-af/jcore/JColorManager.h>

class SearchColorManager : public JColorManager
{
public:

	SearchColorManager();

	virtual ~SearchColorManager();

	virtual bool	GetColorID(const JString& name, JColorID* id) override;
};

#endif
