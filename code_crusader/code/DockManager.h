/******************************************************************************
 DockManager.h

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#ifndef _H_DockManager
#define _H_DockManager

#include <jx-af/jx/JXDockManager.h>

class DockManager : public JXDockManager
{
public:

	DockManager();

	virtual ~DockManager();

	virtual	bool	CanDockAll() const;
	virtual void		DockAll();

private:

	// not allowed

	DockManager(const DockManager& source);
	const DockManager& operator=(const DockManager& source);
};

#endif
