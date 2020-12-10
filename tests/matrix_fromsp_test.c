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

void test_sptSparseTensorToMatrix1(void)
{
    int result;
    sptSparseTensor tsr = {
        .nmodes = 2,
        .sortorder = (sptIndex[]){ 0, 1 },
        .ndims = (sptIndex[]){ 2, 3 },
        .nnz = 3,
        .inds = (sptIndexVector[]){
            (sptIndexVector){
                .len = 3,
                .cap = 3,
                .data = (sptIndex[]){ 0, 1, 1 },
            },
            (sptIndexVector){
                .len = 3,
                .cap = 3,
                .data = (sptIndex[]){ 1, 0, 1 },
            },
        },
        .values = (sptValueVector){
            .len = 3,
            .cap = 3,
            .data = (sptValue[]){ 42, 43, 44 },
        },
    };

    sptMatrix mtx;
    result = sptSparseTensorToMatrix(&mtx, &tsr);
    if (result != 0) {
        abort();
    }

    if (mtx.nrows != 2 || mtx.ncols != 3 || mtx.stride != 8) {
        abort();
    }

    static const sptValue reference[2][8] = {
        { 0, 42, 0, 0, 0, 0, 0, 0 },
        { 43, 44, 0, 0, 0, 0, 0, 0 },
    };
    if (memcmp(mtx.values, reference, sizeof reference) != 0) {
        abort();
    }

    sptFreeMatrix(&mtx);
}

void test_sptSparseTensorToMatrix2(void)
{
    int result;
    sptSparseTensor tsr = {
        .nmodes = 3,
        .sortorder = (sptIndex[]){ 0, 1, 2 },
        .ndims = (sptIndex[]){ 2, 3, 4 },
        .nnz = 0,
        .inds = (sptIndexVector[]){
            (sptIndexVector){
                .len = 0,
                .cap = 0,
                .data = NULL,
            },
            (sptIndexVector){
                .len = 0,
                .cap = 0,
                .data = NULL,
            },
            (sptIndexVector){
                .len = 0,
                .cap = 0,
                .data = NULL,
            },
        },
        .values = (sptValueVector){
            .len = 0,
            .cap = 0,
            .data = NULL,
        },
    };

    sptMatrix mtx;
    result = sptSparseTensorToMatrix(&mtx, &tsr);

    if (result == 0) {
        abort();
    }
}

int main(void)
{
    test_sptSparseTensorToMatrix1();
    test_sptSparseTensorToMatrix2();
    return 0;
}
