#include "common.hpp"
#include <catch.hpp>
#include <command.hpp>

using namespace Slicer;

bool areIndexesMonoIncreasing(const Document& doc)
{
    for (unsigned int i = 0; i < doc.numberOfPages(); ++i)
        if (doc.getPage(i)->getDocumentIndex() != i || doc.getPage(i)->indexInFile() != i)
            return false;

    return true;
}

SCENARIO("Moving 1 page across different places of a document using the Command abstraction")
{
    GIVEN("A multipage PDF document with 15 pages")
    {
        auto multipagePdfFile = Gio::File::create_for_path(multipage1Path);
        Document doc{multipagePdfFile};
        REQUIRE(doc.numberOfPages() == 15);

        WHEN("The first page is moved to the 8th place")
        {
            MovePageCommand command{doc, 0, 7};
            command.execute();

            THEN("The 8th page of the document should be the 1st page of the file")
            REQUIRE(doc.getPage(7)->indexInFile() == 0);

            THEN("The 7th page of the document should be the 8th page of the file")
            REQUIRE(doc.getPage(6)->indexInFile() == 7);

            THEN("The 9th page of the document should be the 9th page of the file")
            REQUIRE(doc.getPage(8)->indexInFile() == 8);

            THEN("The first page of the document should be the second page of the file")
            REQUIRE(doc.getPage(0)->indexInFile() == 1);

            WHEN("The command is undone")
            {
                command.undo();

                THEN("Indexes should start from 0 and be monotonically increasing")
                REQUIRE(areIndexesMonoIncreasing(doc));
            }
        }

        WHEN("The last page is moved to the 8th place")
        {
            MovePageCommand command{doc, 14, 7};
            command.execute();

            THEN("The 8th page of the document should be the 15th page of the file")
            REQUIRE(doc.getPage(7)->indexInFile() == 14);

            THEN("The 9th page of the document should be the 8th page of the file")
            REQUIRE(doc.getPage(8)->indexInFile() == 7);

            THEN("The last page of the document should be the 14th page of the file")
            REQUIRE(doc.getPage(14)->indexInFile() == 13);

            WHEN("The command is undone")
            {
                command.undo();

                THEN("Indexes should start from 0 and be monotonically increasing")
                REQUIRE(areIndexesMonoIncreasing(doc));
            }
        }

        WHEN("The 8th page is moved to the 1st place")
        {
            MovePageCommand command{doc, 7, 0};
            command.execute();

            THEN("The first page of the document should be the 8th page of the file")
            REQUIRE(doc.getPage(0)->indexInFile() == 7);

            THEN("The second page of the document should be the first page of the file")
            REQUIRE(doc.getPage(1)->indexInFile() == 0);

            THEN("The 8th page of the document should be the 7th page of the file")
            REQUIRE(doc.getPage(7)->indexInFile() == 6);

            THEN("The 9th page of the document should be the 9th page of the file")
            REQUIRE(doc.getPage(8)->indexInFile() == 8);

            WHEN("The command is undone")
            {
                command.undo();

                THEN("Indexes should start from 0 and be monotonically increasing")
                REQUIRE(areIndexesMonoIncreasing(doc));
            }
        }

        WHEN("The 8th page is moved to the last place")
        {
            MovePageCommand command{doc, 7, 14};
            command.execute();

            THEN("The 15th page of the document should be the 8th page of the file")
            REQUIRE(doc.getPage(14)->indexInFile() == 7);

            THEN("The 14th page of the document should be the last page of the file")
            REQUIRE(doc.getPage(13)->indexInFile() == 14);

            THEN("The 8th page of the document should be the 9th page of the file")
            REQUIRE(doc.getPage(7)->indexInFile() == 8);

            WHEN("The command is undone")
            {
                command.undo();

                THEN("Indexes should start from 0 and be monotonically increasing")
                REQUIRE(areIndexesMonoIncreasing(doc));
            }
        }
    }
}

SCENARIO("Moving 2 adjacent pages across different places of a document, using the Command abstraction")
{
    GIVEN("A multipage PDF document with 15 pages")
    {
        auto multipagePdfFile = Gio::File::create_for_path(multipage1Path);
        Document doc{multipagePdfFile};
        REQUIRE(doc.numberOfPages() == 15);

        WHEN("The first 2 pages are moved to the 8th place")
        {
            MovePageRangeCommand command{doc, 0, 1, 7};
            command.execute();

            THEN("The 8th and 9th pages of the document should be the 1st and 2nd pages of the file, respectively")
            {
                REQUIRE(doc.getPage(7)->indexInFile() == 0);
                REQUIRE(doc.getPage(8)->indexInFile() == 1);
            }

            THEN("The 1st page of the document should be the 3rd page of the file")
            REQUIRE(doc.getPage(0)->indexInFile() == 2);

            THEN("The 6th and 7th pages of the document should be the 8th and 9th pages of the file, respectively")
            {
                REQUIRE(doc.getPage(5)->indexInFile() == 7);
                REQUIRE(doc.getPage(6)->indexInFile() == 8);
            }

            THEN("The 10th page of the document should be the 10th page of the file")
            REQUIRE(doc.getPage(9)->indexInFile() == 9);

            WHEN("The command is undone")
            {
                command.undo();

                THEN("Indexes should start from 0 and be monotonically increasing")
                REQUIRE(areIndexesMonoIncreasing(doc));
            }
        }

        WHEN("The first 2 pages are moved to the end of the document")
        {
            MovePageRangeCommand command{doc, 0, 1, 13};
            command.execute();

            THEN("The 14th and 15th pages of the document should be the 1st and 2nd pages of the file, respectively")
            {
                REQUIRE(doc.getPage(13)->indexInFile() == 0);
                REQUIRE(doc.getPage(14)->indexInFile() == 1);
            }

            THEN("The 1st page of the document should be the 3rd page of the file")
            REQUIRE(doc.getPage(0)->indexInFile() == 2);

            THEN("The 13th page of the document should be the 15th page of the file")
            REQUIRE(doc.getPage(12)->indexInFile() == 14);

            WHEN("The command is undone")
            {
                command.undo();

                THEN("Indexes should start from 0 and be monotonically increasing")
                REQUIRE(areIndexesMonoIncreasing(doc));
            }
        }

        WHEN("The last 2 pages are moved to the 8th place")
        {
            MovePageRangeCommand command{doc, 13, 14, 7};
            command.execute();

            THEN("The 8th and 9th pages of the document should be the 14th and 15th pages of the file, respectively")
            {
                REQUIRE(doc.getPage(7)->indexInFile() == 13);
                REQUIRE(doc.getPage(8)->indexInFile() == 14);
            }

            THEN("The last page of the document should be the 13th page of the file")
            REQUIRE(doc.getPage(14)->indexInFile() == 12);

            THEN("The 10th page of the document should be the 8th page of the file")
            REQUIRE(doc.getPage(9)->indexInFile() == 7);

            WHEN("The command is undone")
            {
                command.undo();

                THEN("Indexes should start from 0 and be monotonically increasing")
                REQUIRE(areIndexesMonoIncreasing(doc));
            }
        }

        WHEN("The last 2 pages are moved to the beggining")
        {
            MovePageRangeCommand command{doc, 13, 14, 0};
            command.execute();

            THEN("The 1st and 2nd pages of the document should be the 14th and 15th pages of the file")
            {
                REQUIRE(doc.getPage(0)->indexInFile() == 13);
                REQUIRE(doc.getPage(1)->indexInFile() == 14);
            }

            THEN("The last page of the document should be the 13th page of the file")
            REQUIRE(doc.getPage(14)->indexInFile() == 12);

            THEN("The 3rd page of the document should be the 1st page of the file")
            REQUIRE(doc.getPage(2)->indexInFile() == 0);

            WHEN("The command is undone")
            {
                command.undo();

                THEN("Indexes should start from 0 and be monotonically increasing")
                REQUIRE(areIndexesMonoIncreasing(doc));
            }
        }

        WHEN("The first 2 pages are moved to the 2nd and 3rd places in the document")
        {
            MovePageRangeCommand command{doc, 0, 1, 1};
            command.execute();

            THEN("The 2nd and 3rd pages of the document should be the 1st and 2nd pages of the file, respectively")
            {
                REQUIRE(doc.getPage(1)->indexInFile() == 0);
                REQUIRE(doc.getPage(2)->indexInFile() == 1);
            }

            THEN("The 1st page of the document should be the 3rd page of the file")
            REQUIRE(doc.getPage(0)->indexInFile() == 2);

            THEN("The 4rd page of the document should be the 4rd page of the file")
            REQUIRE(doc.getPage(3)->indexInFile() == 3);

            WHEN("The command is undone")
            {
                command.undo();

                THEN("Indexes should start from 0 and be monotonically increasing")
                REQUIRE(areIndexesMonoIncreasing(doc));
            }
        }
    }
}
