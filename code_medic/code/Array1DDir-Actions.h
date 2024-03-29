// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_Array1DDir_Actions
#define _H_Array1DDir_Actions

static const JUtf8Byte* kActionsMenuStr =
"* %i DisplayAsCStringAction::Array1DDir %l"
"|* %i Display1DArray::Array1DDir"
"|* %i Plot1DArray::Array1DDir"
"|* %i Display2DArray::Array1DDir %l"
"|* %i WatchVarValue::Array1DDir"
"|* %i WatchVarLocation::Array1DDir %l"
"|* %i ExamineMemory::Array1DDir"
"|* %i DisasmMemory::Array1DDir %l"
"|* %i __SaveWindowSizeAsDefault::Array1DDir"
;

#include "Array1DDir-Actions-enum.h"

#ifndef _H_medic_show_1d_array
#define _H_medic_show_1d_array
#include "medic_show_1d_array.xpm"
#endif
#ifndef _H_medic_show_2d_plot
#define _H_medic_show_2d_plot
#include "medic_show_2d_plot.xpm"
#endif
#ifndef _H_medic_show_2d_array
#define _H_medic_show_2d_array
#include "medic_show_2d_array.xpm"
#endif
#ifndef _H_medic_show_memory
#define _H_medic_show_memory
#include "medic_show_memory.xpm"
#endif

static void ConfigureActionsMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#a");
	}
	menu->SetItemImage(kDisplay1DArrayCmd + offset, medic_show_1d_array);
	menu->SetItemImage(kPlot1DArrayCmd + offset, medic_show_2d_plot);
	menu->SetItemImage(kDisplay2DArrayCmd + offset, medic_show_2d_array);
	menu->SetItemImage(kExamineMemCmd + offset, medic_show_memory);
};

#endif
