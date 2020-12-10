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

#include <stdio.h>
#include <ParTI.h>

int main(int argc, char *argv[]) {
    char ifname[1000];
    FILE *fo;
    sptSparseTensor tsr;

    if(argc != 3) {
        printf("Usage: %s input output\n\n", argv[0]);
        return 1;
    }

    strcpy(ifname, argv[1]);
    sptAssert(sptLoadSparseTensor(&tsr, 1, ifname) == 0);

    fo = fopen(argv[2], "w");
    sptAssert(fo != NULL);
    sptAssert(sptDumpSparseTensor(&tsr, 1, fo) == 0);
    fclose(fo);

    sptFreeSparseTensor(&tsr);

    return 0;
}
