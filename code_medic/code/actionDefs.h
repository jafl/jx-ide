/******************************************************************************
 actionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_actionDefs
#define _H_actionDefs

#include <jx-af/jx/jXActionDefs.h>	// for convenience

// File menu

#define kOpenSourceFileAction  "GDBOpenSourceFile"
#define kLoadConfigAction      "GDBLoadConfig"
#define kSaveConfigAction      "GDBSaveConfig"
#define kEditSourceFileAction  "GDBEditSourceFile"
#define kSaveHistoryAction     "GDBSaveHistory"
#define kSaveHistoryAsAction   "GDBSaveHistoryAs"

#define kRestartDebuggerAction "GDBRestartDebugger"
#define kChangeDebuggerAction  "GDBChangeDebugger"

// Debug menu

#define kChooseBinaryAction          "GDBChooseBinary"
#define kReloadBinaryAction          "GDBReloadBinary"
#define kChooseCoreAction            "GDBChooseCore"
#define kChooseProcessAction         "GDBChooseProcess"
#define kSetArgumentsAction          "GDBSetArguments"
#define kEnableAllBreakpointsAction  "GDBEnableAllBreakpoints"
#define kDisableAllBreakpointsAction "GDBDisableAllBreakpoints"
#define kClearAllBreakpointsAction   "GDBClearAllBreakpoints"
#define kDisplayVariableAction       "GDBDisplayVariable"
#define kDisplayAsCStringAction      "GDBDisplayAsCStringAction"
#define kDisplay1DArrayAction        "GDBDisplay1DArray"
#define kPlot1DArrayAction           "GDBPlot1DArray"
#define kDisplay2DArrayAction        "GDBDisplay2DArray"
#define kExamineMemoryAction         "GDBExamineMemory"
#define kDisasmMemoryAction          "GDBDisasmMemory"
#define kDisasmFunctionAction        "GDBDisasmFunction"
#define kRunProgramAction            "GDBRunProgram"
#define kStopProgramAction           "GDBStopProgram"
#define kKillProgramAction           "GDBKillProgram"
#define kNextInstrAction             "GDBNextInstr"
#define kStepIntoAction              "GDBStepInto"
#define kFinishSubAction             "GDBFinishSub"
#define kContinueRunAction           "GDBContinueRun"
#define kNextAsmInstrAction          "GDBNextInstrAsm"
#define kStepIntoAsmAction           "GDBStepIntoAsm"
#define kPrevInstrAction             "GDBPrevInstr"
#define kReverseStepIntoAction       "GDBReverseStepInto"
#define kReverseFinishSubAction      "GDBReverseFinishSub"
#define kReverseContinueRunAction    "GDBReverseContinueRun"

// Windows menu

#define kShowCommandLineAction    "GDBShowCommandLine"
#define kShowCurrentSourceAction  "GDBShowCurrentSource"
#define kShowThreadsAction        "GDBShowThreads"
#define kShowStackTraceAction     "GDBShowStackTrace"
#define kShowBreakpointsAction    "GDBShowBreakpoints"
#define kShowVariablesAction      "GDBShowVariables"
#define kShowLocalVariablesAction "GDBShowLocalVariables"
#define kShowCurrentAsmAction     "GDBShowCurrentAsm"
#define kShowRegistersAction      "GDBShowRegisters"
#define kShowFileListAction       "GDBShowFileList"
#define kShowDebugInfoAction      "GDBShowDebugInfo"

// Search menu

#define kBalanceGroupingSymbolAction "GDBBalanceGroupingSymbol"
#define kGoToLineAction              "GDBGoToLine"
#define kGoToCurrentLineAction       "GDBGoToCurrentLine"

// Variables

#define kNewExpressionAction    "GDBNewExpression"
#define kRemoveExpressionAction "GDBRemoveExpression"
#define kWatchVarValueAction    "GDBWatchVarValue"
#define kWatchVarLocationAction "GDBWatchVarLocation"
#define kCopyFullVarNameAction  "GDBCopyFullVarName"
#define kCopyVarValueAction     "GDBCopyVarValue"

#endif
