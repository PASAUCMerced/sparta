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

#include <ParTI.h>
#include <string.h>

static void test_sptNewMatrix(void)
{
    int result;
    sptMatrix mtx;

    result = sptNewMatrix(&mtx, 5, 11);
    if (result != 0) {
        abort();
    }

    if (mtx.nrows != 5 || mtx.ncols != 11 || mtx.cap != 5 || mtx.stride != 16) {
        abort();
    }

    sptFreeMatrix(&mtx);
}

static void test_sptRandomizeMatrix(void)
{
    int result;
    sptMatrix mtx;

    result = sptNewMatrix(&mtx, 42, 24);
    if (result != 0) {
        abort();
    }

    result = sptRandomizeMatrix(&mtx, mtx.nrows, mtx.ncols);
    if (result != 0) {
        abort();
    }

    sptFreeMatrix(&mtx);
}

static void test_sptIdentityMatrix(void)
{
    int result;
    sptMatrix mtx;

    result = sptNewMatrix(&mtx, 5, 5);
    if (result != 0) {
        abort();
    }

    memset(mtx.values, 0, 5 * 8 * sizeof *mtx.values);

    result = sptIdentityMatrix(&mtx);
    if (result != 0) {
        abort();
    }

    static const sptValue reference[5][8] = {
        { 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0 }
    };
    if (memcmp(mtx.values, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeMatrix(&mtx);
}

static void test_sptConstantMatrix(void)
{
    int result;
    sptMatrix mtx;

    result = sptNewMatrix(&mtx, 5, 5);
    if (result != 0) {
        abort();
    }

    memset(mtx.values, 0, 5 * 8 * sizeof *mtx.values);

    result = sptConstantMatrix(&mtx, 42);
    if (result != 0) {
        abort();
    }

    static const sptValue reference[5][8] = {
        { 42, 42, 42, 42, 42, 0, 0, 0 },
        { 42, 42, 42, 42, 42, 0, 0, 0 },
        { 42, 42, 42, 42, 42, 0, 0, 0 },
        { 42, 42, 42, 42, 42, 0, 0, 0 },
        { 42, 42, 42, 42, 42, 0, 0, 0 }
    };
    if (memcmp(mtx.values, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeMatrix(&mtx);
}

static void test_sptCopyMatrix(void)
{
    int result;
    sptMatrix mtx1, mtx2;

    result = sptNewMatrix(&mtx1, 3, 9);
    if (result != 0) {
        abort();
    }

    static const sptValue reference[3][16] = {
        { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0 },
        { 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0, 0, 0 },
        { 33, 34, 35, 36, 37, 38, 39, 40, 41, 0, 0, 0, 0, 0, 0, 0 }
    };
    memcpy(mtx1.values, reference, sizeof reference);

    result = sptCopyMatrix(&mtx2, &mtx1);
    if (result != 0) {
        abort();
    }

    if (memcmp(mtx2.values, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeMatrix(&mtx2);
    sptFreeMatrix(&mtx1);
}

static void test_sptAppendMatrix(void)
{
    int result;
    sptMatrix mtx;

    result = sptNewMatrix(&mtx, 2, 2);
    if (result != 0) {
        abort();
    }

    static const sptValue reference[3][8] = {
        { 42, 43, 0, 0, 0, 0, 0, 0 },
        { 44, 45, 0, 0, 0, 0, 0, 0 },
        { 46, 47, 0, 0, 0, 0, 0, 0 }
    };
    memcpy(mtx.values, reference, 2 * sizeof reference[0]);

    result = sptAppendMatrix(&mtx, reference[2]);
    if (result != 0) {
        abort();
    }

    if (mtx.nrows != 3) {
        abort();
    }

    if (memcmp(mtx.values, reference, 18 * sizeof reference[0][0]) != 0) {
        abort();
    }

    sptFreeMatrix(&mtx);
}

static void test_sptResizeMatrix(void)
{
    int result;
    sptMatrix mtx;

    result = sptNewMatrix(&mtx, 2, 2);
    if (result != 0) {
        abort();
    }

    static const sptValue reference[2][8] = {
        { 42, 43, 0, 0, 0, 0, 0, 0 },
        { 44, 45, 0, 0, 0, 0, 0, 0 },
    };
    memcpy(mtx.values, reference, sizeof reference);

    result = sptResizeMatrix(&mtx, 3);
    if (result != 0) {
        abort();
    }

    if (mtx.nrows != 3) {
        abort();
    }

    if (memcmp(mtx.values, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeMatrix(&mtx);
}

int main(void)
{
    test_sptNewMatrix();
    test_sptRandomizeMatrix();
    test_sptIdentityMatrix();
    test_sptConstantMatrix();
    test_sptCopyMatrix();
    test_sptAppendMatrix();
    test_sptResizeMatrix();
    return 0;
}
