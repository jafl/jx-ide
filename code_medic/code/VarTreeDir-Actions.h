// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_VarTreeDir_Actions
#define _H_VarTreeDir_Actions

static const JUtf8Byte* kActionsMenuStr =
"* %i NewExpression::VarTreeDir"
"|* %i RemoveExpression::VarTreeDir %l"
"|* %i Display1DArray::VarTreeDir"
"|* %i Plot1DArray::VarTreeDir"
"|* %i Display2DArray::VarTreeDir %l"
"|* %i WatchVarValue::VarTreeDir"
"|* %i WatchVarLocation::VarTreeDir %l"
"|* %i ExamineMemory::VarTreeDir"
"|* %i DisasmMemory::VarTreeDir"
;

#include "VarTreeDir-Actions-enum.h"


static void ConfigureActionsMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#s");
	}
};

#endif
