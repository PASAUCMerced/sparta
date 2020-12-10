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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../error/error.h"


/**
 * Dum a dense matrix to file
 *
 * @param mtx   a valid pointer to a sptMatrix variable
 * @param fp a file pointer
 *
 */
int sptDumpMatrix(sptMatrix *mtx, FILE *fp) {
    int iores;
    sptIndex nrows = mtx->nrows;
    sptIndex ncols = mtx->ncols;
    sptIndex stride = mtx->stride;
    iores = fprintf(fp, "%"PARTI_PRI_INDEX " x %"PARTI_PRI_INDEX " matrix\n", nrows, ncols);
    spt_CheckOSError(iores < 0, "Mtx Dump");
    for(sptIndex i=0; i < nrows; ++i) {
      for(sptIndex j=0; j < ncols; ++j) {
          iores = fprintf(fp, "%.2"PARTI_PRI_VALUE "\t", mtx->values[i * stride + j]);
          spt_CheckOSError(iores < 0, "Mtx Dump");
      }
      iores = fprintf(fp, "\n");
    }
    iores = fprintf(fp, "\n");
    return 0;
}


/**
 * Dum a dense rank matrix to file
 *
 * @param mtx   a valid pointer to a sptMatrix variable
 * @param fp a file pointer
 *
 */
int sptDumpRankMatrix(sptRankMatrix *mtx, FILE *fp) {
    int iores;
    sptIndex nrows = mtx->nrows;
    sptElementIndex ncols = mtx->ncols;
    sptElementIndex stride = mtx->stride;
    iores = fprintf(fp, "%"PARTI_PRI_INDEX " x %"PARTI_PRI_ELEMENT_INDEX " matrix\n", nrows, ncols);
    spt_CheckOSError(iores < 0, "RankMtx Dump");
    for(sptIndex i=0; i < nrows; ++i) {
      for(sptElementIndex j=0; j < ncols; ++j) {
          iores = fprintf(fp, "%.2"PARTI_PRI_VALUE "\t", mtx->values[i * stride + j]);
          spt_CheckOSError(iores < 0, "RankMtx Dump");
      }
      iores = fprintf(fp, "\n");
    }
    iores = fprintf(fp, "\n");
    return 0;
}
