/******************************************************************************
 ProjectTableInput.h

	Copyright © 2006 by John Lindal.

 ******************************************************************************/

#ifndef _H_ProjectTableInput
#define _H_ProjectTableInput

#include <jx-af/jx/JXFileInput.h>

class ProjectTable;

class ProjectTableInput : public JXFileInput
{
public:

	ProjectTableInput(ProjectTable* table, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	~ProjectTableInput() override;

	void	HandleKeyPress(const JUtf8Character& c,
						   const int keySym, const JXKeyModifiers& modifiers) override;

private:

	ProjectTable*	itsProjectTable;
};

#endif
