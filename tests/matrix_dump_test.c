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

// fmemopen
#define _POSIX_C_SOURCE 200809L

#include <ParTI.h>
#include <string.h>
#include <stdio.h>

static void test_sptDumpMatrix(void)
{
    int result;
    sptMatrix mtx;

    char* buf = calloc(1, 4096);
    FILE* fp = fmemopen(buf, 4096, "wb");

    result = sptNewMatrix(&mtx, 2, 3);
    if (result != 0) {
        abort();
    }

    static const sptValue reference[2][8] = {
        { 1, 2, 3, 0, 0, 0, 0, 0 },
        { 4, 5, 6, 0, 0, 0, 0, 0 },
    };
    memcpy(mtx.values, reference, sizeof reference);

    result = sptDumpMatrix(&mtx, fp);
    if (result != 0) {
        abort();
    }

    fclose(fp);
    if (strcmp(buf, "2 x 3 matrix\n1.00\t2.00\t3.00\t\n4.00\t5.00\t6.00\t\n\n") != 0) {
        abort();
    }

    sptFreeMatrix(&mtx);
}

int main(void)
{
    test_sptDumpMatrix();
    return 0;
}
