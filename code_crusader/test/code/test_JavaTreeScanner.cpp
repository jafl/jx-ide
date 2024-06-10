/******************************************************************************
 test_JavaTreeScanner.cpp

	Test Java class parser.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "JavaTree.h"
#include "Class.h"
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

class TestJavaTree : public JavaTree
{
public:

	TestJavaTree()
		:
		JavaTree(nullptr, 0)
	{ };

	void ParseFile(const JString& fileName, const JFAID_t id) override;
};

void
TestJavaTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	JavaTree::ParseFile(fileName, id);
}

JTEST(Basic)
{
	JArray<JFAID_t> deadFileList;

	TestJavaTree tree;
	tree.PrepareForUpdate(false);
	tree.ParseFile("./data/tree/java/AccessibilityController.java", 1);
	tree.ParseFile("./data/tree/java/PowerNotificationWarnings.java", 2);
	tree.ParseFile("./data/tree/java/PizzaStatus.java", 3);
	tree.UpdateFinished(deadFileList);

	const TestJavaTree& constTree  = tree;
	const JPtrArray<Class>& list = constTree.GetClasses();
	JAssertEqual(8, list.GetElementCount());

	JSize found = 0;
	for (Class* c : list)
		{
		if (c->GetFullName() == "com.android.systemui.statusbar.policy.AccessibilityController")
			{
			JAssertFalse(c->IsAbstract());
			JAssertTrue(c->HasParents());
			JAssertEqual(2, c->GetParentCount());
			}
		else if (c->GetFullName() == "com.android.systemui.statusbar.policy.AccessibilityController.AccessibilityStateChangedCallback")
			{
			JAssertTrue(c->IsAbstract());
			}
		else if (c->GetFullName() == "com.android.systemui.power.PowerNotificationWarnings" ||
				 c->GetFullName() == "com.android.systemui.power.PowerNotificationWarnings.Receiver")
			{
			JAssertFalse(c->IsAbstract());
			JAssertTrue(c->HasParents());
			JAssertEqual(1, c->GetParentCount());
			}
		else
			{
			JAssertEqual(Class::kGhostType, c->GetDeclareType());
			}

		JAssertFalse(c->IsTemplate());

		for (const JUtf8Byte* n :
			{
				"com.android.systemui.statusbar.policy.AccessibilityController",
				"com.android.systemui.statusbar.policy.AccessibilityController.AccessibilityStateChangedCallback",
				"android.view.accessibility.AccessibilityManager.AccessibilityStateChangeListener",
				"android.view.accessibility.AccessibilityManager.TouchExplorationStateChangeListener",
				"com.android.systemui.power.PowerNotificationWarnings",
				"PowerUI.WarningsUI",
				"com.android.systemui.power.PowerNotificationWarnings.Receiver",
				"android.content.BroadcastReceiver"
			})
			{
			if (n == c->GetFullName())
				{
				found++;
				break;
				}
			}
		}
	JAssertEqual(8, found);
}

JTEST(Template)
{
	JArray<JFAID_t> deadFileList;

	TestJavaTree tree;
	tree.PrepareForUpdate(false);
	tree.ParseFile("./data/tree/java/DateVerifier.java", 1);
	tree.ParseFile("./data/tree/java/IdVerifier.java", 2);
	tree.ParseFile("./data/tree/java/StartDateVerifier.java", 3);
	tree.UpdateFinished(deadFileList);

	const TestJavaTree& constTree  = tree;
	const JPtrArray<Class>& list = constTree.GetClasses();
	JAssertEqual(5, list.GetElementCount());

	JSize found = 0;
	for (Class* c : list)
		{
		if (c->GetFullName() == "com.opencsv.bean.BeanVerifier" ||
			c->GetFullName() == "test.foo.Test1")
			{
			JAssertEqual(Class::kGhostType, c->GetDeclareType());
			}
		else if (c->GetFullName() == "test.csv.verifier.StartDateVerifier")
			{
			JAssertFalse(c->IsTemplate());
			JAssertTrue(c->HasParents());
			JAssertEqual(2, c->GetParentCount());
			}
		else
			{
			JAssertTrue(c->IsTemplate());
			JAssertTrue(c->HasParents());
			JAssertEqual(1, c->GetParentCount());
			}

		for (const JUtf8Byte* n :
			{
				"test.csv.verifier.DateVerifier",
				"test.csv.verifier.IdVerifier",
				"test.csv.verifier.StartDateVerifier",
				"com.opencsv.bean.BeanVerifier",
				"test.foo.Test1"
			})
			{
			if (n == c->GetFullName())
				{
				found++;
				break;
				}
			}
		}
	JAssertEqual(5, found);
}

bool
InUpdateThread()
{
	return true;
}
