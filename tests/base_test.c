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
#include <stdlib.h>
#include <string.h>

//Tests
static void test_sptBytesString(void)
{
    char* buf;

    buf = sptBytesString(42);
    if (strcmp(buf, "42.00 B") != 0) {
        abort();
    }
    free(buf);

    buf = sptBytesString(42000);
    if (strcmp(buf, "41.02 KiB") != 0) {
        abort();
    }
    free(buf);

    buf = sptBytesString(42000000);
    if (strcmp(buf, "40.05 MiB") != 0) {
        abort();
    }
    free(buf);

    buf = sptBytesString(42000000000);
    if (strcmp(buf, "39.12 GiB") != 0) {
        abort();
    }
    free(buf);

    buf = sptBytesString(42000000000000);
    if (strcmp(buf, "38.20 TiB") != 0) {
        abort();
    }
    free(buf);

    buf = sptBytesString(42000000000000000);
    if (strcmp(buf, "38198.78 TiB") != 0) {
        abort();
    }
    free(buf);
}

static void test_sptRandomValue(void)
{
    while (sptRandomValue() < 0) {
    }
    while (sptRandomValue() >= 0) {
    }
}

int main(void)
{
    test_sptBytesString();
    test_sptRandomValue();
    return 0;
}
