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

static void test_sptNewRankMatrix(void)
{
    int result;
    sptRankMatrix mtx;

    result = sptNewRankMatrix(&mtx, 5, 11);
    if (result != 0) {
        abort();
    }

    if (mtx.nrows != 5 || mtx.ncols != 11 || mtx.cap != 5 || mtx.stride != 16) {
        abort();
    }

    sptFreeRankMatrix(&mtx);
}

static void test_sptRandomizeRankMatrix(void)
{
    int result;
    sptRankMatrix mtx;

    result = sptNewRankMatrix(&mtx, 42, 24);
    if (result != 0) {
        abort();
    }

    result = sptRandomizeRankMatrix(&mtx, mtx.nrows, mtx.ncols);
    if (result != 0) {
        abort();
    }

    sptFreeRankMatrix(&mtx);
}

static void test_sptConstantRankMatrix(void)
{
    int result;
    sptRankMatrix mtx;

    result = sptNewRankMatrix(&mtx, 5, 5);
    if (result != 0) {
        abort();
    }

    memset(mtx.values, 0, 5 * 8 * sizeof *mtx.values);

    result = sptConstantRankMatrix(&mtx, 42);
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

    sptFreeRankMatrix(&mtx);
}

int main(void)
{
    test_sptNewRankMatrix();
    test_sptRandomizeRankMatrix();
    test_sptConstantRankMatrix();
    return 0;
}
