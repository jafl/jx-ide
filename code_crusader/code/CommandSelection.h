/******************************************************************************
 CommandSelection.h

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#ifndef _H_CommandSelection
#define _H_CommandSelection

#include <jx-af/jx/JXSelectionManager.h>
#include "CommandManager.h"

class CommandTable;

class CommandSelection : public JXSelectionData
{
public:

	CommandSelection(JXDisplay* display, CommandTable* table,
					 const CommandManager::CmdInfo& cmdInfo);

	~CommandSelection() override;

	static const JUtf8Byte*	GetCommandXAtomName();

protected:

	void	AddTypes(const Atom selectionName) override;
	bool	ConvertData(const Atom requestType, Atom* returnType,
						unsigned char** data, JSize* dataLength,
						JSize* bitsPerBlock) const override;

private:

	JString			itsData;
	CommandTable*	itsTable;	// not owned; can be nullptr
	JIndex			itsSrcRowIndex;

	Atom	itsCBCommandXAtom;
};

#endif
