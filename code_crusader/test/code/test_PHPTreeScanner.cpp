/******************************************************************************
 test_PHPTreeScanner.cpp

	Test PHP class parser.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "PHPTree.h"
#include "Class.h"
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

class TestPHPTree : public PHPTree
{
public:

	TestPHPTree()
		:
		PHPTree(nullptr, 0)
	{ };

	void ParseFile(const JString& fileName, const JFAID_t id) override;
};

void
TestPHPTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	PHPTree::ParseFile(fileName, id);
}

JTEST(Basic)
{
	JArray<JFAID_t> deadFileList;

	TestPHPTree tree;
	tree.PrepareForUpdate(false);
	tree.ParseFile("./data/tree/php/A1.php", 1);
	tree.ParseFile("./data/tree/php/B1.php", 2);
	tree.ParseFile("./data/tree/php/InvalidArgumentException.php", 3);
	tree.UpdateFinished(deadFileList);

	const TestPHPTree& constTree   = tree;
	const JPtrArray<Class>& list = constTree.GetClasses();
	JAssertEqual(5, list.GetElementCount());

	JSize found = 0;
	for (Class* c : list)
		{
		if (c->GetFullName() == "Zend\\Authentication\\Adapter\\DbTable\\Exception\\InvalidArgumentException")
			{
			JAssertTrue(c->HasParents());
			JAssertEqual(2, c->GetParentCount());
			}
		else if (c->GetFullName() == "foo\\B\\B1")
			{
			JAssertTrue(c->HasParents());
			JAssertEqual(1, c->GetParentCount());
			}
		else if (c->GetFullName() == "A\\A1")
			{
			JAssertTrue(c->IsAbstract());
			}
		else
			{
			JAssertEqual(Class::kGhostType, c->GetDeclareType());
			}

		for (const JUtf8Byte* n :
			{
				"A\\A1",
				"Exception\\InvalidArgumentException",
				"ExceptionInterface",
				"foo\\B\\B1",
				"Zend\\Authentication\\Adapter\\DbTable\\Exception\\InvalidArgumentException"
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
