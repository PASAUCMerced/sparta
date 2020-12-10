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

static void test_sptNewElementIndexVector(void)
{
    int result;
    sptElementIndexVector vec;

    result = sptNewElementIndexVector(&vec, 1, 0);
    if (result != 0) {
        abort();
    }

    if (vec.len != 1) {
        abort();
    }
    if (vec.cap != 2) {
        abort();
    }

    sptFreeElementIndexVector(&vec);
}

static void test_sptConstantElementIndexVector(void)
{
    int result;
    sptElementIndexVector vec;

    result = sptNewElementIndexVector(&vec, 8, 0);
    if (result != 0) {
        abort();
    }

    sptConstantElementIndexVector(&vec, 42);

    static const sptElementIndex reference[8] = { 42, 42, 42, 42, 42, 42, 42, 42 };

    if (memcmp(vec.data, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeElementIndexVector(&vec);
}

static void test_sptCopyElementIndexVector(void)
{
    int result;
    sptElementIndexVector vec1, vec2;

    result = sptNewElementIndexVector(&vec1, 8, 0);
    if (result != 0) {
        abort();
    }

    static const sptElementIndex reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec1.data, reference, sizeof reference);

    result = sptCopyElementIndexVector(&vec2, &vec1);
    if (result != 0) {
        abort();
    }

    if (memcmp(vec2.data, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeElementIndexVector(&vec2);
    sptFreeElementIndexVector(&vec1);
}

static void test_sptAppendElementIndexVector(void)
{
    int result;
    sptElementIndexVector vec;

    result = sptNewElementIndexVector(&vec, 7, 0);
    if (result != 0) {
        abort();
    }

    static const sptElementIndex reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec.data, reference, 7 * sizeof reference[0]);

    result = sptAppendElementIndexVector(&vec, reference[7]);
    if (result != 0) {
        abort();
    }

    if (memcmp(vec.data, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeElementIndexVector(&vec);
}

static void test_sptAppendElementIndexVectorWithVector(void)
{
    int result;
    sptElementIndexVector vec1, vec2;

    result = sptNewElementIndexVector(&vec1, 4, 0);
    if (result != 0) {
        abort();
    }

    static const sptElementIndex reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec1.data, reference, 4 * sizeof reference[0]);

    result = sptNewElementIndexVector(&vec2, 4, 0);
    if (result != 0) {
        abort();
    }

    memcpy(vec2.data, reference + 4, 4 * sizeof reference[0]);

    result = sptAppendElementIndexVectorWithVector(&vec1, &vec2);
    if (result != 0) {
        abort();
    }

    if (memcmp(vec1.data, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeElementIndexVector(&vec2);
    sptFreeElementIndexVector(&vec1);
}

static void test_sptResizeElementIndexVector(void)
{
    int result;
    sptElementIndexVector vec;

    result = sptNewElementIndexVector(&vec, 8, 0);
    if (result != 0) {
        abort();
    }

    static const sptElementIndex reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec.data, reference, sizeof reference);

    result = sptResizeElementIndexVector(&vec, 4);
    if (result != 0) {
        abort();
    }

    if (vec.len != 4) {
        abort();
    }
    if (vec.cap != 4) {
        abort();
    }
    if (memcmp(vec.data, reference, 4 * sizeof reference[0]) != 0) {
        abort();
    }

    sptFreeElementIndexVector(&vec);
}

int main(void)
{
    test_sptNewElementIndexVector();
    test_sptConstantElementIndexVector();
    test_sptCopyElementIndexVector();
    test_sptCopyElementIndexVector();
    test_sptAppendElementIndexVector();
    test_sptAppendElementIndexVectorWithVector();
    test_sptResizeElementIndexVector();
    return 0;
}
