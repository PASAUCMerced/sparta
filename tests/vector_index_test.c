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

static void test_sptNewIndexVector(void)
{
    int result;
    sptIndexVector vec;

    result = sptNewIndexVector(&vec, 1, 0);
    if (result != 0) {
        abort();
    }

    if (vec.len != 1) {
        abort();
    }
    if (vec.cap != 2) {
        abort();
    }

    sptFreeIndexVector(&vec);
}

static void test_sptConstantIndexVector(void)
{
    int result;
    sptIndexVector vec;

    result = sptNewIndexVector(&vec, 8, 0);
    if (result != 0) {
        abort();
    }

    sptConstantIndexVector(&vec, 42);

    static const sptIndex reference[8] = { 42, 42, 42, 42, 42, 42, 42, 42 };

    if (memcmp(vec.data, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeIndexVector(&vec);
}

static void test_sptCopyIndexVector(void)
{
    int result;
    sptIndexVector vec1, vec2;

    result = sptNewIndexVector(&vec1, 8, 0);
    if (result != 0) {
        abort();
    }

    static const sptIndex reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec1.data, reference, sizeof reference);

    result = sptCopyIndexVector(&vec2, &vec1, 1);
    if (result != 0) {
        abort();
    }

    if (memcmp(vec2.data, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeIndexVector(&vec2);
    sptFreeIndexVector(&vec1);
}

static void test_sptAppendIndexVector(void)
{
    int result;
    sptIndexVector vec;

    result = sptNewIndexVector(&vec, 7, 0);
    if (result != 0) {
        abort();
    }

    static const sptIndex reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec.data, reference, 7 * sizeof reference[0]);

    result = sptAppendIndexVector(&vec, reference[7]);
    if (result != 0) {
        abort();
    }

    if (memcmp(vec.data, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeIndexVector(&vec);
}

static void test_sptAppendIndexVectorWithVector(void)
{
    int result;
    sptIndexVector vec1, vec2;

    result = sptNewIndexVector(&vec1, 4, 0);
    if (result != 0) {
        abort();
    }

    static const sptIndex reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec1.data, reference, 4 * sizeof reference[0]);

    result = sptNewIndexVector(&vec2, 4, 0);
    if (result != 0) {
        abort();
    }

    memcpy(vec2.data, reference + 4, 4 * sizeof reference[0]);

    result = sptAppendIndexVectorWithVector(&vec1, &vec2);
    if (result != 0) {
        abort();
    }

    if (memcmp(vec1.data, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeIndexVector(&vec2);
    sptFreeIndexVector(&vec1);
}

static void test_sptResizeIndexVector(void)
{
    int result;
    sptIndexVector vec;

    result = sptNewIndexVector(&vec, 8, 0);
    if (result != 0) {
        abort();
    }

    static const sptIndex reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec.data, reference, sizeof reference);

    result = sptResizeIndexVector(&vec, 4);
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

    sptFreeIndexVector(&vec);
}

int main(void)
{
    test_sptNewIndexVector();
    test_sptConstantIndexVector();
    test_sptCopyIndexVector();
    test_sptCopyIndexVector();
    test_sptAppendIndexVector();
    test_sptAppendIndexVectorWithVector();
    test_sptResizeIndexVector();
    return 0;
}
