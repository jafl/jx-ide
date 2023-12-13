// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_TreeDirector_Tree
#define _H_TreeDirector_Tree

static const JUtf8Byte* kTreeMenuStr =
"* %i EditSearchPaths::TreeDirector"
"|* %i UpdateClassTree::TreeDirector"
"|* %i MinimizeMILinkLength::TreeDirector %l"
"|* %i OpenSelectedFiles::TreeDirector"
"|* %i OpenClassFnList::TreeDirector %l"
"|* %i CollapseClasses::TreeDirector"
"|* %i ExpandClasses::TreeDirector"
"|* %i ExpandAllClasses::TreeDirector %l"
"|* %i SelectParentClass::TreeDirector"
"|* %i SelectDescendantClass::TreeDirector"
"|* %i CopyClassName::TreeDirector %l"
"|* %i FindFunction::TreeDirector"
;

enum {
	kEditSearchPathsCmd=1,
	kUpdateCurrentCmd,
	kForceMinMILinksCmd,
	kTreeOpenSourceCmd,
	kTreeOpenFnListCmd,
	kTreeCollapseCmd,
	kTreeExpandCmd,
	kTreeExpandAllCmd,
	kTreeSelParentsCmd,
	kTreeSelDescendantsCmd,
	kCopySelNamesCmd,
	kFindFnCmd,
};

#ifndef _H_jx_af_image_jx_jx_edit_copy
#define _H_jx_af_image_jx_jx_edit_copy
#include <jx-af/image/jx/jx_edit_copy.xpm>
#endif

static void ConfigureTreeMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#t", JString::kNoCopy));
	}
	menu->SetItemImage(kCopySelNamesCmd + offset, jx_edit_copy);
};

#endif
