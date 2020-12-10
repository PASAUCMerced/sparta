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

#ifndef PARTI_SSPTENSOR_H
#define PARTI_SSPTENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ParTI.h>
#include "../error/error.h"

int spt_SemiSparseTensorAppend(sptSemiSparseTensor *tsr, const sptIndex indices[], sptValue value);
int spt_SemiSparseTensorCompareIndices(const sptSemiSparseTensor *tsr1, sptNnzIndex ind1, const sptSemiSparseTensor *tsr2, sptNnzIndex ind2);
int spt_SemiSparseTensorMergeValues(sptSemiSparseTensor *tsr);

double spt_SemiSparseTensorNorm(const sptSemiSparseTensor *X);

int spt_SemiSparseTensorSetMode(
    sptSemiSparseTensor       *dest,
    const sptSemiSparseTensor *src,
    sptIndex                    newmode
);
int spt_SemiSparseTensorToSparseMatrixCSR(
    sptValue                  *csrVal,
    sptNnzIndex                        *csrRowPtr,
    sptIndex                        *csrColInd,
    const sptSemiSparseTensor  *tsr
);

#ifdef __cplusplus
}
#endif

#endif
