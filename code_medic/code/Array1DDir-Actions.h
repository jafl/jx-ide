// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_Array1DDir_Actions
#define _H_Array1DDir_Actions

static const JUtf8Byte* kActionsMenuStr =
"* %i Display1DArray::Array1DDir"
"|* %i Plot1DArray::Array1DDir"
"|* %i Display2DArray::Array1DDir %l"
"|* %i WatchVarValue::Array1DDir"
"|* %i WatchVarLocation::Array1DDir %l"
"|* %i ExamineMemory::Array1DDir"
"|* %i DisasmMemory::Array1DDir %l"
"|* %i __SaveWindowSizeAsDefault::Array1DDir"
;

#include "Array1DDir-Actions-enum.h"


static void ConfigureActionsMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#a");
	}
};

#endif
