/******************************************************************************
 CClass.h

	Copyright © 1996-99 John Lindal.

 ******************************************************************************/

#ifndef _H_CClass
#define _H_CClass

#include "Class.h"

class CClass : public Class
{
public:

	CClass(const JString& name, const DeclareType declType,
			 const JFAID_t fileID, Tree* tree);
	CClass(std::istream& input, const JFileVersion vers, Tree* tree);

	virtual ~CClass();

	virtual void	ViewSource() const override;
	virtual void	ViewHeader() const override;

protected:

	virtual Class*	NewGhost(const JString& name, Tree* tree) override;
};

#endif
