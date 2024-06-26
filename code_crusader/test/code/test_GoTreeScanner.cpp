/******************************************************************************
 test_GoTreeScanner.cpp

	Test Go class parser.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "GoTree.h"
#include "Class.h"
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

class TestGoTree : public GoTree
{
public:

	TestGoTree()
		:
		GoTree(nullptr, 0)
	{ };

	void ParseFile(const JString& fileName, const JFAID_t id) override;
};

void
TestGoTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	GoTree::ParseFile(fileName, id);
}

JTEST(Basic)
{
	JArray<JFAID_t> deadFileList;

	TestGoTree tree;
	tree.PrepareForUpdate(false);
	tree.ParseFile("./data/tree/go/a.go", 1);
	tree.ParseFile("./data/tree/go/b.go", 2);
	tree.ParseFile("./data/tree/go/c.go", 3);
	tree.UpdateFinished(deadFileList);

	const TestGoTree& constTree    = tree;
	const JPtrArray<Class>& list = constTree.GetClasses();
	JAssertEqual(10, list.GetElementCount());

	JSize found = 0;
	for (Class* c : list)
		{
		if (c->GetFullName() == "a.A1" || c->GetFullName() == "c.C1")
			{
			JAssertFalse(c->IsAbstract());
			JAssertFalse(c->HasParents());
			}
		else if (c->GetFullName() == "a.A2" || c->GetFullName() == "b.b3")
			{
			JAssertTrue(c->IsAbstract());
			JAssertFalse(c->HasParents());
			}
		else if (c->GetFullName() == "b.B1" || c->GetFullName() == "b.b2")
			{
			JAssertFalse(c->IsAbstract());
			JAssertTrue(c->HasParents());
			JAssertEqual(1, c->GetParentCount());
			}
		else if (c->GetFullName() == "c.C2")
			{
			JAssertFalse(c->IsAbstract());
			JAssertTrue(c->HasParents());
			JAssertEqual(4, c->GetParentCount());
			}
		else if (c->GetFullName() == "c.c3")
			{
			JAssertTrue(c->IsAbstract());
			JAssertTrue(c->HasParents());
			JAssertEqual(3, c->GetParentCount());
			}
		else if (c->GetFullName() == "D1")
			{
			JAssertEqual(Class::kGhostType, c->GetDeclareType());
			}

		for (const JUtf8Byte* n :
			{
				"a.A1",
				"a.A2",
				"b.B1",
				"b.b2",
				"b.B3",
				"b.b4",
				"c.C1",
				"c.C2",
				"c.c3",
				"D1"
			})
			{
			if (n == c->GetFullName())
				{
				found++;
				break;
				}
			}
		}
	JAssertEqual(10, found);
}

bool
InUpdateThread()
{
	return true;
}
