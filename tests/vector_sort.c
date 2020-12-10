/*
    This file is part of ParTI!.

    ParTI! is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    ParTI! is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with ParTI!.
    If not, see <http://www.gnu.org/licenses/>.
*/

#include "../src/error/error.h"
#include <ParTI.h>
#include <stdlib.h>
#include <string.h>

static void test_sptQuickSortNnzIndexArray(void)
{
    int result;
    sptNnzIndexVector vec;

    result = sptNewNnzIndexVector(&vec, 8, 0);
    if (result != 0) {
        abort();
    }

    static const sptNnzIndex source[8] = { 3, 4, 7, 1, 8, 5, 2, 6 };
    memcpy(vec.data, source, sizeof source);

    sptQuickSortNnzIndexArray(vec.data, 0, vec.len);

    static const sptNnzIndex ordered[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    if (memcmp(vec.data, ordered, sizeof ordered) != 0) {
        abort();
    }

    sptFreeNnzIndexVector(&vec);
}

int main(void)
{
    test_sptQuickSortNnzIndexArray();
    return 0;
}
