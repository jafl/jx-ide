/******************************************************************************
 CBDockManager.h

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBDockManager
#define _H_CBDockManager

#include <jx-af/jx/JXDockManager.h>

class CBDockManager : public JXDockManager
{
public:

	CBDockManager();

	virtual ~CBDockManager();

	virtual	bool	CanDockAll() const;
	virtual void		DockAll();

private:

	// not allowed

	CBDockManager(const CBDockManager& source);
	const CBDockManager& operator=(const CBDockManager& source);
};

#endif
