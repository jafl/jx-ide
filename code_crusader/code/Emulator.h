/******************************************************************************
 Emulator.h

	Copyright © 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_Emulator
#define _H_Emulator

#include <jx-af/jcore/jTypes.h>

class JTEKeyHandler;
class TextEditor;

// Do not change the file type values once they are assigned because
// they are stored in the prefs file.

enum Emulator
{
	kNoEmulator = 0,
	kVIEmulator,			// = kLastEmulator	// special

	// When you add new types, be sure to increment the prefs version!

	kFirstEmulator = kNoEmulator,
	kLastEmulator  = kVIEmulator
};

const JSize kEmulatorCount = kLastEmulator+1;

bool	InstallEmulator(const Emulator type, TextEditor* editor,
							  JTEKeyHandler** handler);

std::istream& operator>>(std::istream& input, Emulator& type);
std::ostream& operator<<(std::ostream& output, const Emulator type);

#endif
