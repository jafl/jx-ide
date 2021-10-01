/******************************************************************************
 ProjectNodeType.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_ProjectNodeType
#define _H_ProjectNodeType

#include <jx-af/jcore/jTypes.h>

// Do not change the file type values once they are assigned because
// they are stored in the prefs file.

enum ProjectNodeType
{
	kRootNT = 0,
	kGroupNT,
	kFileNT,
	kLibraryNT
};

const JSize kNTCount = kLibraryNT+1;

std::istream& operator>>(std::istream& input, ProjectNodeType& type);
std::ostream& operator<<(std::ostream& output, const ProjectNodeType type);

#endif
