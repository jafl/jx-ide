/******************************************************************************
 CBPTPrinter.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPTPrinter
#define _H_CBPTPrinter

#include <jx-af/jx/JXPTPrinter.h>
#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JString.h>

class CBPTPrintSetupDialog;

class CBPTPrinter : public JXPTPrinter, public JPrefObject
{
public:

	CBPTPrinter();

	virtual ~CBPTPrinter();

	void	SetHeaderName(const JString& name);

	// printing parameters

	bool	WillPrintHeader() const;
	void		ShouldPrintHeader(const bool print);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual JSize	GetHeaderLineCount() const;
	virtual void	PrintHeader(std::ostream& output, const JIndex pageIndex);

	virtual JXPTPrintSetupDialog*
		CreatePrintSetupDialog(const Destination destination,
							   const JString& printCmd, const JString& fileName,
							   const bool printLineNumbers);

	virtual bool	EndUserPrintSetup(const JBroadcaster::Message& message,
										  bool* changed);

private:

	bool	itsPrintHeaderFlag;
	JString		itsHeaderName;

	CBPTPrintSetupDialog*	itsCBPrintSetupDialog;
};


/******************************************************************************
 SetHeaderName

 ******************************************************************************/

inline void
CBPTPrinter::SetHeaderName
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
CBPTPrinter::WillPrintHeader()
	const
{
	return itsPrintHeaderFlag;
}

inline void
CBPTPrinter::ShouldPrintHeader
	(
	const bool print
	)
{
	itsPrintHeaderFlag = print;
}

#endif
