// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_Generic_Debug
#define _H_Generic_Debug

static const JUtf8Byte* kDebugMenuStr =
"* %i ChooseBinary::Generic"
"|* %i ReloadBinary::Generic"
"|* %i ChooseCore::Generic"
"|* %i ChooseProcess::Generic"
"|* %i SetArguments::Generic %l"
"|* %i RestartDebugger::Generic %l"
"|* %i DisplayVariable::Generic"
"|* %i Display1DArray::Generic"
"|* %i Plot1DArray::Generic"
"|* %i Display2DArray::Generic %l"
"|* %i WatchVarValue::Generic"
"|* %i WatchVarLocation::Generic %l"
"|* %i ExamineMemory::Generic"
"|* %i DisasmMemory::Generic"
"|* %i DisasmFunction::Generic %l"
"|* %i RunProgram::Generic"
"|* %i StopProgram::Generic"
"|* %i KillProgram::Generic %l"
"|* %i NextInstr::Generic"
"|* %i StepInto::Generic"
"|* %i FinishSub::Generic"
"|* %i ContinueRun::Generic %l"
"|* %i NextInstrAsm::Generic"
"|* %i StepIntoAsm::Generic %l"
"|* %i PrevInstr::Generic"
"|* %i ReverseStepInto::Generic"
"|* %i ReverseFinishSub::Generic"
"|* %i ReverseContinueRun::Generic %l"
"|* %i ClearAllBreakpoints::Generic %l"
;

#include "Generic-Debug-enum.h"

#ifndef _H_jx_af_image_jx_jx_executable_small
#define _H_jx_af_image_jx_jx_executable_small
#include <jx-af/image/jx/jx_executable_small.xpm>
#endif
#ifndef _H_medic_choose_core
#define _H_medic_choose_core
#include "medic_choose_core.xpm"
#endif
#ifndef _H_medic_attach_process
#define _H_medic_attach_process
#include "medic_attach_process.xpm"
#endif
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
#ifndef _H_medic_run_program
#define _H_medic_run_program
#include "medic_run_program.xpm"
#endif
#ifndef _H_medic_stop_program
#define _H_medic_stop_program
#include "medic_stop_program.xpm"
#endif
#ifndef _H_jx_af_image_jx_jx_edit_clear
#define _H_jx_af_image_jx_jx_edit_clear
#include <jx-af/image/jx/jx_edit_clear.xpm>
#endif
#ifndef _H_medic_next
#define _H_medic_next
#include "medic_next.xpm"
#endif
#ifndef _H_medic_step
#define _H_medic_step
#include "medic_step.xpm"
#endif
#ifndef _H_medic_finish
#define _H_medic_finish
#include "medic_finish.xpm"
#endif
#ifndef _H_medic_continue
#define _H_medic_continue
#include "medic_continue.xpm"
#endif
#ifndef _H_medic_nexti
#define _H_medic_nexti
#include "medic_nexti.xpm"
#endif
#ifndef _H_medic_stepi
#define _H_medic_stepi
#include "medic_stepi.xpm"
#endif
#ifndef _H_medic_reverse_next
#define _H_medic_reverse_next
#include "medic_reverse_next.xpm"
#endif
#ifndef _H_medic_reverse_step
#define _H_medic_reverse_step
#include "medic_reverse_step.xpm"
#endif
#ifndef _H_medic_reverse_finish
#define _H_medic_reverse_finish
#include "medic_reverse_finish.xpm"
#endif
#ifndef _H_medic_reverse_continue
#define _H_medic_reverse_continue
#include "medic_reverse_continue.xpm"
#endif

static void ConfigureDebugMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#d");
	}
	menu->SetItemImage(kSelectBinCmd + offset, jx_executable_small);
	menu->SetItemImage(kSelectCoreCmd + offset, medic_choose_core);
	menu->SetItemImage(kSelectProcessCmd + offset, medic_attach_process);
	menu->SetItemImage(kDisplay1DArrayCmd + offset, medic_show_1d_array);
	menu->SetItemImage(kPlot1DArrayCmd + offset, medic_show_2d_plot);
	menu->SetItemImage(kDisplay2DArrayCmd + offset, medic_show_2d_array);
	menu->SetItemImage(kExamineMemCmd + offset, medic_show_memory);
	menu->SetItemImage(kRunCmd + offset, medic_run_program);
	menu->SetItemImage(kStopCmd + offset, medic_stop_program);
	menu->SetItemImage(kKillCmd + offset, jx_edit_clear);
	menu->SetItemImage(kNextCmd + offset, medic_next);
	menu->SetItemImage(kStepCmd + offset, medic_step);
	menu->SetItemImage(kFinishCmd + offset, medic_finish);
	menu->SetItemImage(kContCmd + offset, medic_continue);
	menu->SetItemImage(kNextAsmCmd + offset, medic_nexti);
	menu->SetItemImage(kStepAsmCmd + offset, medic_stepi);
	menu->SetItemImage(kPrevCmd + offset, medic_reverse_next);
	menu->SetItemImage(kReverseStepCmd + offset, medic_reverse_step);
	menu->SetItemImage(kReverseFinishCmd + offset, medic_reverse_finish);
	menu->SetItemImage(kReverseContCmd + offset, medic_reverse_continue);
};

#endif
