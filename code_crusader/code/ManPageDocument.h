/******************************************************************************
 ManPageDocument.h

	Interface for the ManPageDocument class

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_ManPageDocument
#define _H_ManPageDocument

#include "TextDocument.h"

class ManPageDocument : public TextDocument
{
public:

	static bool	Create(ManPageDocument** doc, const JString& pageName,
						   const JString& pageIndex = JString::empty,
						   const bool apropos = false);

	virtual ~ManPageDocument();

protected:

	ManPageDocument(const JString& pageName, const JString& pageIndex,
					  const bool apropos, ManPageDocument** trueDoc);

	static JString	GetCmd1(const JString& pageName, const JString& pageIndex);
	static JString	GetCmd2(const JString& pageName);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsIgnoreNameChangedFlag;

	static JPtrArray<JString>			theManCmdList;
	static JPtrArray<TextDocument>	theManDocList;

private:

	static void	RemoveFromManPageList(ManPageDocument* doc);

	// not allowed

	ManPageDocument(const ManPageDocument& source);
	const ManPageDocument& operator=(const ManPageDocument& source);
};

#endif
