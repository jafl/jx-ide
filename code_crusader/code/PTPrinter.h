/******************************************************************************
 PTPrinter.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_PTPrinter
#define _H_PTPrinter

#include <jx-af/jx/JXPTPrinter.h>
#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JString.h>

class PTPrintSetupDialog;

class PTPrinter : public JXPTPrinter, public JPrefObject
{
public:

	PTPrinter();

	~PTPrinter();

	void	SetHeaderName(const JString& name);

	// printing parameters

	bool	WillPrintHeader() const;
	void	ShouldPrintHeader(const bool print);

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	JSize	GetHeaderLineCount() const override;
	void	PrintHeader(std::ostream& output, const JIndex pageIndex) override;

	JXPTPrintSetupDialog*
		CreatePrintSetupDialog(const Destination destination,
							   const JString& printCmd, const JString& fileName,
							   const bool printLineNumbers) override;

	bool	EndUserPrintSetup(const JBroadcaster::Message& message,
							  bool* changed) override;

private:

	bool	itsPrintHeaderFlag;
	JString	itsHeaderName;

	PTPrintSetupDialog*	itsCBPrintSetupDialog;
};


/******************************************************************************
 SetHeaderName

 ******************************************************************************/

inline void
PTPrinter::SetHeaderName
	(
	const JString& name
	)
{
	itsHeaderName = name;
}

/******************************************************************************
 SetHeaderName

 ******************************************************************************/

inline bool
PTPrinter::WillPrintHeader()
	const
{
	return itsPrintHeaderFlag;
}

inline void
PTPrinter::ShouldPrintHeader
	(
	const bool print
	)
{
	itsPrintHeaderFlag = print;
}

#endif
