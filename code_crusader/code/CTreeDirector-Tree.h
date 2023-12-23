// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_CTreeDirector_Tree
#define _H_CTreeDirector_Tree

static const JUtf8Byte* kTreeMenuStr =
"* %i ConfigureCTreeCPP::CTreeDirector"
"|* %i EditSearchPaths::CTreeDirector"
"|* %i UpdateClassTree::CTreeDirector"
"|* %i MinimizeMILinkLength::CTreeDirector %l"
"|* %i OpenSelectedFiles::CTreeDirector"
"|* %i OpenComplFiles::CTreeDirector"
"|* %i OpenClassFnList::CTreeDirector %l"
"|* %i CreateDerivedClass::CTreeDirector %l"
"|* %i CollapseClasses::CTreeDirector"
"|* %i ExpandClasses::CTreeDirector"
"|* %i ExpandAllClasses::CTreeDirector %l"
"|* %i SelectParentClass::CTreeDirector"
"|* %i SelectDescendantClass::CTreeDirector"
"|* %i CopyClassName::CTreeDirector %l"
"|* %i FindFunction::CTreeDirector"
;

enum {
	kEditCPPMacrosCmd=1,
	kEditSearchPathsCmd,
	kUpdateCurrentCmd,
	kForceMinMILinksCmd,
	kTreeOpenSourceCmd,
	kTreeOpenHeaderCmd,
	kTreeOpenFnListCmd,
	kCreateDerivedClassCmd,
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