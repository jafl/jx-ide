// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_CompileDocument_Errors
#define _H_CompileDocument_Errors

static const JUtf8Byte* kErrorsMenuStr =
"* %i __FirstError::CompileDocument %l"
"|* %i __PreviousError::CompileDocument"
"|* %i __NextError::CompileDocument %l"
"|* %i __OpenSelectionToError::CompileDocument"
;

#include "CompileDocument-Errors-enum.h"


static void ConfigureErrorsMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#e");
	}
};

#endif
