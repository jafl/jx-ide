/******************************************************************************
 Emulator.cpp

	Copyright © 2010 by John Lindal.

 ******************************************************************************/

#include "Emulator.h"
#include "VIKeyHandler.h"
#include "TextEditor.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 InstallEmulator

 ******************************************************************************/

bool
InstallEmulator
	(
	const Emulator	type,
	TextEditor*		editor,
	JTEKeyHandler**		handler
	)
{
	*handler = nullptr;

	if (type == kVIEmulator)
	{
		*handler = jnew VIKeyHandler(editor);
	}

	editor->SetKeyHandler(*handler);
	return *handler != nullptr;
}

/******************************************************************************
 Stream operators

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&	input,
	Emulator&	type
	)
{
	long temp;
	input >> temp;
	type = (Emulator) temp;
	assert( kFirstEmulator <= type && type <= kLastEmulator );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&			output,
	const Emulator	type
	)
{
	output << (long) type;
	return output;
}
