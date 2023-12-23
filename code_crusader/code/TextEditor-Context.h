// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_TextEditor_Context
#define _H_TextEditor_Context

static const JUtf8Byte* kContextMenuStr =
"* %i __Cut::TextEditor"
"|* %i __Copy::TextEditor"
"|* %i __Paste::TextEditor %l"
"|* %i __ShiftLeft::TextEditor"
"|* %i __ShiftRight::TextEditor %l"
"|* %i __Complete::TextEditor"
"|* %i __RunMacro::TextEditor"
"|* %i __Scripts::TextEditor %l"
"|* %i __FindBackwards::TextEditor"
"|* %i __FindForward::TextEditor %l"
"|* %i __FindAsSymbol::TextEditor"
"|* %i __FindAsSymbolNoContext::TextEditor"
"|* %i __FindAsSymbolManPageOnly::TextEditor %l"
"|* %i __OpenAsFileNameOrURL::TextEditor"
"|* %i __ShowInFileManager::TextEditor %l"
"|* %i __BalanceClosestGroupingSymbols::TextEditor"
"|* %i __PlaceBookmark::TextEditor"
;

enum {
	kContextCutCmd=1,
	kContextCopyCmd,
	kContextPasteCmd,
	kContextShiftLeftCmd,
	kContextShiftRightCmd,
	kContextCompleteCmd,
	kContextMacroCmd,
	kContextScriptSubmenuIndex,
	kContextFindSelBackCmd,
	kContextFindSelFwdCmd,
	kContextFindSymbolCmd,
	kContextFindSymbolNoContextCmd,
	kContextFindSymbolManPageCmd,
	kContextOpenSelAsFileCmd,
	kShowInFileMgrCmd,
	kContextBalanceCmd,
	kContextPlaceBookmarkCmd,
};

#ifndef _H_jx_af_image_jx_jx_edit_cut
#define _H_jx_af_image_jx_jx_edit_cut
#include <jx-af/image/jx/jx_edit_cut.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_copy
#define _H_jx_af_image_jx_jx_edit_copy
#include <jx-af/image/jx/jx_edit_copy.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_paste
#define _H_jx_af_image_jx_jx_edit_paste
#include <jx-af/image/jx/jx_edit_paste.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_shift_left
#define _H_jx_af_image_jx_jx_edit_shift_left
#include <jx-af/image/jx/jx_edit_shift_left.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_shift_right
#define _H_jx_af_image_jx_jx_edit_shift_right
#include <jx-af/image/jx/jx_edit_shift_right.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_find_selection_backwards
#define _H_jx_af_image_jx_jx_find_selection_backwards
#include <jx-af/image/jx/jx_find_selection_backwards.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_find_selection_forward
#define _H_jx_af_image_jx_jx_find_selection_forward
#include <jx-af/image/jx/jx_find_selection_forward.xpm>
#endif
#ifndef _H_jcc_balance_braces
#define _H_jcc_balance_braces
#include "jcc_balance_braces.xpm"
#endif

static void ConfigureContextMenu(JXTextMenu* menu, const int offset = 0) {
	menu->SetItemImage(kContextCutCmd + offset, jx_edit_cut);
	menu->SetItemImage(kContextCopyCmd + offset, jx_edit_copy);
	menu->SetItemImage(kContextPasteCmd + offset, jx_edit_paste);
	menu->SetItemImage(kContextShiftLeftCmd + offset, jx_edit_shift_left);
	menu->SetItemImage(kContextShiftRightCmd + offset, jx_edit_shift_right);
	menu->SetItemImage(kContextFindSelBackCmd + offset, jx_find_selection_backwards);
	menu->SetItemImage(kContextFindSelFwdCmd + offset, jx_find_selection_forward);
	menu->SetItemImage(kContextBalanceCmd + offset, jcc_balance_braces);
};

#endif