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

#include "../src/error/error.h"
#include <ParTI.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void test_sptDumpElementIndexVector(void)
{
    int result;
    sptElementIndexVector vec;

    char* buf = calloc(1, 4096);
    FILE* fp = fmemopen(buf, 4096, "wb");

    result = sptNewElementIndexVector(&vec, 8, 0);
    if (result != 0) {
        abort();
    }

    static const sptElementIndex reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec.data, reference, sizeof reference);

    result = sptDumpElementIndexVector(&vec, fp);
    if (result != 0) {
        abort();
    }

    fclose(fp);
    if (strcmp(buf, "sptElementIndexVector length: 8\n1\t2\t3\t4\t5\t6\t7\t8\t\n") != 0) {
        abort();
    }

    sptFreeElementIndexVector(&vec);
}

static void test_sptDumpIndexVector(void)
{
    int result;
    sptIndexVector vec;

    char* buf = calloc(1, 4096);
    FILE* fp = fmemopen(buf, 4096, "wb");

    result = sptNewIndexVector(&vec, 8, 0);
    if (result != 0) {
        abort();
    }

    static const sptIndex reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec.data, reference, sizeof reference);

    result = sptDumpIndexVector(&vec, fp);
    if (result != 0) {
        abort();
    }

    fclose(fp);
    if (strcmp(buf, "sptIndexVector length: 8\n1\t2\t3\t4\t5\t6\t7\t8\t\n") != 0) {
        abort();
    }

    sptFreeIndexVector(&vec);
}

static void test_sptDumpBlockIndexVector(void)
{
    int result;
    sptBlockIndexVector vec;

    char* buf = calloc(1, 4096);
    FILE* fp = fmemopen(buf, 4096, "wb");

    result = sptNewBlockIndexVector(&vec, 8, 0);
    if (result != 0) {
        abort();
    }

    static const sptBlockIndex reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec.data, reference, sizeof reference);

    result = sptDumpBlockIndexVector(&vec, fp);
    if (result != 0) {
        abort();
    }

    fclose(fp);
    if (strcmp(buf, "sptBlockIndexVector length: 8\n1\t2\t3\t4\t5\t6\t7\t8\t\n") != 0) {
        abort();
    }

    sptFreeBlockIndexVector(&vec);
}

static void test_sptDumpNnzIndexVector(void)
{
    int result;
    sptNnzIndexVector vec;

    char* buf = calloc(1, 4096);
    FILE* fp = fmemopen(buf, 4096, "wb");

    result = sptNewNnzIndexVector(&vec, 8, 0);
    if (result != 0) {
        abort();
    }

    static const sptNnzIndex reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec.data, reference, sizeof reference);

    result = sptDumpNnzIndexVector(&vec, fp);
    if (result != 0) {
        abort();
    }

    fclose(fp);
    if (strcmp(buf, "sptNnzIndexVector length: 8\n1\t2\t3\t4\t5\t6\t7\t8\t\n") != 0) {
        abort();
    }

    sptFreeNnzIndexVector(&vec);
}

static void test_sptDumpValueVector(void)
{
    int result;
    sptValueVector vec;

    char* buf = calloc(1, 4096);
    FILE* fp = fmemopen(buf, 4096, "wb");

    result = sptNewValueVector(&vec, 8, 0);
    if (result != 0) {
        abort();
    }

    static const sptValue reference[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    memcpy(vec.data, reference, sizeof reference);

    result = sptDumpValueVector(&vec, fp);
    if (result != 0) {
        abort();
    }

    fclose(fp);
    if (strcmp(buf, "sptValueVector length: 8\n1.000000\t2.000000\t3.000000\t4.000000\t5.000000\t6.000000\t7.000000\t8.000000\t\n") != 0) {
        abort();
    }

    sptFreeValueVector(&vec);
}

static void test_sptDumpIndexArray(void)
{
    int result;
    static const sptIndex arr[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

    char* buf = calloc(1, 4096);
    FILE* fp = fmemopen(buf, 4096, "wb");

    result = sptDumpIndexArray(arr, sizeof arr / sizeof arr[0], fp);
    if (result != 0) {
        abort();
    }

    fclose(fp);
    if (strcmp(buf, "sptIndex array length: 8\n1\t2\t3\t4\t5\t6\t7\t8\t\n") != 0) {
        abort();
    }
}

static void test_sptDumpNnzIndexArray(void)
{
    int result;
    static const sptNnzIndex arr[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

    char* buf = calloc(1, 4096);
    FILE* fp = fmemopen(buf, 4096, "wb");

    result = sptDumpNnzIndexArray(arr, sizeof arr / sizeof arr[0], fp);
    if (result != 0) {
        abort();
    }

    fclose(fp);
    if (strcmp(buf, "sptNnzIndex array length: 8\n1\t2\t3\t4\t5\t6\t7\t8\t\n") != 0) {
        abort();
    }
}

int main(void)
{
    test_sptDumpElementIndexVector();
    test_sptDumpIndexVector();
    test_sptDumpBlockIndexVector();
    test_sptDumpNnzIndexVector();
    test_sptDumpValueVector();
    test_sptDumpIndexArray();
    test_sptDumpNnzIndexArray();
    return 0;
}
