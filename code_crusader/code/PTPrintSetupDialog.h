/******************************************************************************
 PTPrintSetupDialog.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_PTPrintSetupDialog
#define _H_PTPrintSetupDialog

#include <jx-af/jx/JXPTPrintSetupDialog.h>

class PTPrintSetupDialog : public JXPTPrintSetupDialog
{
public:

	static PTPrintSetupDialog*
		Create(const JXPTPrinter::Destination dest,
			   const JString& printCmd, const JString& fileName,
			   const bool printLineNumbers, const bool printHeader);

	virtual ~PTPrintSetupDialog();

	bool	ShouldPrintHeader() const;

protected:

	PTPrintSetupDialog();

private:

// begin JXLayout

	JXTextCheckbox* itsPrintHeaderCB;

// end JXLayout

private:

	void	BuildWindow(const JXPTPrinter::Destination dest,
						const JString& printCmd, const JString& fileName,
						const bool printLineNumbers, const bool printHeader);
};

#endif
