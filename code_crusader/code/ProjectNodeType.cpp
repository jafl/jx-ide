/******************************************************************************
 ProjectNodeType.cpp

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "ProjectNodeType.h"
#include <jx-af/jcore/jAssert.h>

std::istream&
operator>>
	(
	std::istream&			input,
	ProjectNodeType&	type
	)
{
	long temp;
	input >> temp;
	type = (ProjectNodeType) temp;
	assert( kRootNT <= type && JSize(type) < kNTCount );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&				output,
	const ProjectNodeType	type
	)
{
	output << (long) type;
	return output;
}
