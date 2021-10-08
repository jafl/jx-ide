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

	~DockManager() override;

	bool	CanDockAll() const override;
	void	DockAll() override;
};

#endif
