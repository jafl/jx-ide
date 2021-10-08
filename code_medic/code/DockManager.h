/******************************************************************************
 DockManager.h

	Copyright (C) 2003 by John Lindal.

 ******************************************************************************/

#ifndef _H_DockManager
#define _H_DockManager

#include <jx-af/jx/JXDockManager.h>

class DockManager : public JXDockManager
{
public:

	DockManager();

	~DockManager();

	bool	CanDockAll() const override;
	void	DockAll() override;
};

#endif
