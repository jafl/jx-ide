/******************************************************************************
 PTPrinter.cpp

	BASE CLASS = JXPTPrinter, JPrefObject

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "PTPrinter.h"
#include "PTPrintSetupDialog.h"
#include "globals.h"
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jAssert.h>

const JSize kPrintHeaderLineCount = 4;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 0 was written by JXTEBase::WritePrintPlainTextSetup().

/******************************************************************************
 Constructor

 ******************************************************************************/

PTPrinter::PTPrinter()
	:
	JXPTPrinter(),
	JPrefObject(GetPrefsManager(), kPrintPlainTextID),
	itsPrintHeaderFlag(true)
{
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PTPrinter::~PTPrinter()
{
}

/******************************************************************************
 CreatePrintSetupDialog (virtual protected)

	Derived class can override to create an extended dialog.

 ******************************************************************************/

JXPTPrintSetupDialog*
PTPrinter::CreatePrintSetupDialog
	(
	const Destination	destination,
	const JString&		printCmd,
	const JString&		fileName,
	const bool		printLineNumbers
	)
{
	return PTPrintSetupDialog::Create(destination, printCmd, fileName,
									  printLineNumbers, itsPrintHeaderFlag);
}

/******************************************************************************
 ShouldPrintHeader

 ******************************************************************************/

void
PTPrinter::ShouldPrintHeader
	(
	const bool print
	)
{
	itsPrintHeaderFlag = print;
	if (GetPageHeight() <= GetHeaderLineCount())
	{
		SetPageHeight(GetHeaderLineCount() + 1);
	}
}

/******************************************************************************
 Print header and footer (virtual protected)

	Derived classes can override these functions if they want to
	print a header or a footer.  The default is to do nothing.

 ******************************************************************************/

JSize
PTPrinter::GetHeaderLineCount()
	const
{
	return (itsPrintHeaderFlag ? kPrintHeaderLineCount : 0);
}

void
PTPrinter::PrintHeader
	(
	std::ostream&		output,
	const JIndex	pageIndex
	)
{
	if (itsPrintHeaderFlag)
	{
		const JString dateStr = JGetTimeStamp();

		JString pageStr(pageIndex, 0);
		pageStr.Prepend("Page ");

		itsHeaderName.Print(output);
		output << '\n';

		dateStr.Print(output);
		const JInteger spaceCount =
			GetPageWidth() - dateStr.GetCharacterCount() - pageStr.GetCharacterCount();
		for (JInteger i=1; i<=spaceCount; i++)
		{
			output << ' ';
		}
		pageStr.Print(output);
		output << "\n\n\n";
	}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
PTPrinter::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers == 0)
	{
		JString printCmd;
		JSize pageHeight;
		input >> printCmd >> pageHeight;
		JIgnoreUntil(input, '\1');

		SetPrintCmd(printCmd);
		SetPageHeight(pageHeight);
	}
	else if (vers <= kCurrentSetupVersion)
	{
		input >> JBoolFromString(itsPrintHeaderFlag);
		JXPTPrinter::ReadXPTSetup(input);
	}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
PTPrinter::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << JBoolToString(itsPrintHeaderFlag);

	output << ' ';
	JXPTPrinter::WriteXPTSetup(output);

	output << ' ';
}
