/******************************************************************************
 PSPrinter.cpp

	BASE CLASS = JXPSPrinter, JPrefObject

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "PSPrinter.h"
#include "PSPrintSetupDialog.h"
#include "PTPrinter.h"
#include "TextEditor.h"
#include "globals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

const JSize kUnsetFontSize = 0;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 1 stores itsFontSize

/******************************************************************************
 Constructor

 ******************************************************************************/

PSPrinter::PSPrinter
	(
	JXDisplay* display
	)
	:
	JXPSPrinter(display),
	JPrefObject(GetPrefsManager(), kPrintStyledTextID),
	itsFontSize(kUnsetFontSize),
	itsTE(nullptr)
{
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PSPrinter::~PSPrinter()
{
}

/******************************************************************************
 CreatePrintSetupDialog (virtual protected)

	Derived class can override to create an extended dialog.

 ******************************************************************************/

static const JRegex nxmRegex("[0-9]+x([0-9]+)");

JXPSPrintSetupDialog*
PSPrinter::CreatePrintSetupDialog
	(
	const Destination	destination,
	const JString&		printCmd,
	const JString&		fileName,
	const bool			collate,
	const bool			bw
	)
{
	if (itsFontSize == kUnsetFontSize)
	{
		JString fontName;
		GetPrefsManager()->GetDefaultFont(&fontName, &itsFontSize);

		const JStringMatch m = nxmRegex.Match(fontName, JRegex::kIncludeSubmatches);
		if (!m.IsEmpty())
		{
			const JString hStr = m.GetSubstring(1);
			const bool ok  = hStr.ConvertToUInt(&itsFontSize);
			assert( ok );
			itsFontSize--;
		}
	}

	return PSPrintSetupDialog::Create(destination, printCmd, fileName,
									  collate, bw, itsFontSize,
									  GetPTTextPrinter()->WillPrintHeader());
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
PSPrinter::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
	{
		if (vers >= 1)
		{
			input >> itsFontSize;
		}

		JXPSPrinter::ReadXPSSetup(input);
	}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
PSPrinter::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsFontSize;
	output << ' ';
	JXPSPrinter::WriteXPSSetup(output);
	output << ' ';
}

/******************************************************************************
 OpenDocument (virtual)

	Change the font to ensure monospace.

 ******************************************************************************/

bool
PSPrinter::OpenDocument()
{
	assert( itsTE != nullptr );

	JXGetApplication()->DisplayBusyCursor();	// changing font can take a while

	if (JXPSPrinter::OpenDocument())
	{
		assert( itsFontSize != kUnsetFontSize );
		itsTE->SetFontBeforePrintPS(itsFontSize);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 CloseDocument (virtual)

	Resets the font.

 ******************************************************************************/

void
PSPrinter::CloseDocument()
{
	assert( itsTE != nullptr );

	JXPSPrinter::CloseDocument();
	itsTE->ResetFontAfterPrintPS();
	itsTE = nullptr;
}

/******************************************************************************
 CancelDocument (virtual)

	Resets the font.

 ******************************************************************************/

void
PSPrinter::CancelDocument()
{
	assert( itsTE != nullptr );

	JXPSPrinter::CancelDocument();
	itsTE->ResetFontAfterPrintPS();
	itsTE = nullptr;
}
