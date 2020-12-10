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
#include <math.h>
#include "../error/error.h"


/**
 * Initialize a new dense rank matrix
 *
 * @param mtx   a valid pointer to an uninitialized sptMatrix variable
 * @param nrows the number of rows
 * @param ncols the number of columns
 *
 * The memory layout of this dense matrix is a flat 2D array, with `ncols`
 * rounded up to multiples of 8
 */
int sptNewRankMatrix(sptRankMatrix *mtx, sptIndex const nrows, sptElementIndex const ncols) {
    mtx->nrows = nrows;
    mtx->ncols = ncols;
    mtx->cap = nrows != 0 ? nrows : 1;
    mtx->stride = ((ncols-1)/8+1)*8;
#ifdef _ISOC11_SOURCE
    mtx->values = aligned_alloc(8 * sizeof (sptValue), mtx->cap * mtx->stride * sizeof (sptValue));
#elif _POSIX_C_SOURCE >= 200112L
    {
        int result = posix_memalign((void **) &mtx->values, 8 * sizeof (sptValue), mtx->cap * mtx->stride * sizeof (sptValue));
        if(result != 0) {
            mtx->values = NULL;
        }
    }
#else
    mtx->values = malloc(mtx->cap * mtx->stride * sizeof (sptValue));
#endif
    spt_CheckOSError(!mtx->values, "RankMtx New");
    return 0;
}


/**
 * Build a matrix with random number
 *
 * @param mtx   a pointer to an uninitialized matrix
 * @param nrows fill the specified number of rows
 * @param ncols fill the specified number of columns
 *
 * The matrix is filled with uniform distributed pseudorandom number in [0, 1]
 * The random number will have a precision of 31 bits out of 51 bits
 */
int sptRandomizeRankMatrix(sptRankMatrix *mtx, sptIndex const nrows, sptElementIndex const ncols) 
{
  srand(time(NULL));
  for(sptIndex i=0; i<nrows; ++i)
    for(sptElementIndex j=0; j<ncols; ++j) {
      mtx->values[i * mtx->stride + j] = i + j + 1; //sptRandomValue();
    }
  return 0;
}

/**
 * Fill an existed dense rank matrix with a specified constant
 *
 * @param mtx   a pointer to a valid matrix
 * @param val   a given value constant
 *
 */
int sptConstantRankMatrix(sptRankMatrix *mtx, sptValue const val) {
  for(sptIndex i=0; i<mtx->nrows; ++i)
    for(sptElementIndex j=0; j<mtx->ncols; ++j)
      mtx->values[i * mtx->stride + j] = val;
  return 0;
}


/**
 * Shuffle matrix row indices.
 *
 * @param[in] mtx matrix to be shuffled
 * @param[out] map_inds is the renumbering mapping 
 *
 */
void sptRankMatrixInverseShuffleIndices(sptRankMatrix *mtx, sptIndex * mode_map_inds) {
    /* Renumber matrix rows */
    sptIndex new_i;
    sptValue * tmp_values = malloc(mtx->cap * mtx->stride * sizeof (sptValue));

    for(sptIndex i=0; i<mtx->nrows; ++i) {
        new_i = mode_map_inds[i];
        for(sptElementIndex j=0; j<mtx->ncols; ++j) {
            tmp_values[i * mtx->stride + j] = mtx->values[new_i * mtx->stride + j];
        }
    }

    free(mtx->values);
    mtx->values = tmp_values;
}


/**
 * Release the memory buffer a dense rank matrix is holding
 *
 * @param mtx a pointer to a valid matrix
 *
 * By using `sptFreeMatrix`, a valid matrix would become uninitialized and
 * should not be used anymore prior to another initialization
 */
void sptFreeRankMatrix(sptRankMatrix *mtx) {
    free(mtx->values);
    mtx->nrows = 0;
    mtx->ncols = 0;
    mtx->cap = 0;
    mtx->stride = 0;
}


/* mats (aTa) only stores upper triangle elements. */
int sptRankMatrixDotMulSeqTriangle(sptIndex const mode, sptIndex const nmodes, sptRankMatrix ** mats)
{
    sptIndex const nrows = mats[0]->nrows;
    sptElementIndex const ncols = mats[0]->ncols;
    sptElementIndex const stride = mats[0]->stride;
    for(sptIndex m=1; m<nmodes+1; ++m) {
        assert(mats[m]->ncols == ncols);
        assert(mats[m]->nrows == nrows);
    }

    sptValue * ovals = mats[nmodes]->values;
#ifdef PARTI_USE_OPENMP
    #pragma omp parallel for schedule(static)
#endif
    for(sptIndex i=0; i < nrows; ++i) {
        for(sptElementIndex j=0; j < ncols; ++j) {
            ovals[j * stride + i] = 1.0;
        }
    }


    for(sptIndex m=1; m < nmodes; ++m) {
        sptIndex const pm = (mode + m) % nmodes;
        sptValue const * vals = mats[pm]->values;
#ifdef PARTI_USE_OPENMP
    #pragma omp parallel for schedule(static)
#endif
        for(sptIndex i=0; i < nrows; ++i) {
            for(sptElementIndex j=i; j < ncols; ++j) {
                ovals[i * stride + j] *= vals[i * stride + j];
            }
        }
    }

    /* Copy upper triangle to lower part */
#ifdef PARTI_USE_OPENMP
    #pragma omp parallel for schedule(static)
#endif
    for(sptIndex i=0; i < nrows; ++i) {
        for(sptElementIndex j=0; j < i; ++j) {
            ovals[i * stride + j] = ovals[j * stride + i];
        }
    }
    
    return 0;
}


// Row-major
int sptRankMatrix2Norm(sptRankMatrix * const A, sptValue * const lambda)
{
    sptIndex const nrows = A->nrows;
    sptElementIndex const ncols = A->ncols;
    sptElementIndex const stride = A->stride;
    sptValue * const vals = A->values;
    sptValue * buffer_lambda;
    int nthreads = 1;

#ifdef PARTI_USE_OPENMP
    #pragma omp parallel for schedule(static)
#endif
    for(sptElementIndex j=0; j < ncols; ++j) {
        lambda[j] = 0.0;
    }

#ifdef PARTI_USE_OPENMP
    #pragma omp parallel
    {
        nthreads = omp_get_num_threads();
    }
    buffer_lambda = (sptValue *)malloc(nthreads * ncols * sizeof(sptValue));
#endif


#ifdef PARTI_USE_OPENMP
    #pragma omp parallel
    {
        #pragma omp for schedule(static)
        for(sptNnzIndex j=0; j < (sptNnzIndex)nthreads * ncols; ++j)
                buffer_lambda[j] = 0.0;

        int const tid = omp_get_thread_num();
        int const nthreads = omp_get_num_threads();
        sptValue * loc_lambda = buffer_lambda + tid * ncols;

        #pragma omp for
        for(sptIndex i=0; i < nrows; ++i) {
            for(sptElementIndex j=0; j < ncols; ++j) {
                loc_lambda[j] += vals[i*stride + j] * vals[i*stride + j];
            }
        }

        #pragma omp for schedule(static)
        for(sptElementIndex j=0; j < ncols; ++j) {
            for(sptIndex i=0; i < (sptIndex)nthreads; ++i) {
                lambda[j] += buffer_lambda[i*ncols + j];
            }
        }
    }   /* end parallel pragma */

#else

    for(sptIndex i=0; i < nrows; ++i) {
        for(sptElementIndex j=0; j < ncols; ++j) {
            lambda[j] += vals[i*stride + j] * vals[i*stride + j];
        }
    }

#endif

#ifdef PARTI_USE_OPENMP
        #pragma omp parallel for schedule(static)
#endif
        for(sptElementIndex j=0; j < ncols; ++j) {
            lambda[j] = sqrt(lambda[j]);
        }

#ifdef PARTI_USE_OPENMP
        #pragma omp parallel for
#endif
        for(sptIndex i=0; i < nrows; ++i) {
            for(sptElementIndex j=0; j < ncols; ++j) {
                vals[i*stride + j] /= lambda[j];
            }
        }

    
#ifdef PARTI_USE_OPENMP
    free(buffer_lambda);
#endif

    return 0;
}


// Row-major
int sptRankMatrixMaxNorm(sptRankMatrix * const A, sptValue * const lambda)
{
    sptIndex const nrows = A->nrows;
    sptElementIndex const ncols = A->ncols;
    sptElementIndex const stride = A->stride;
    sptValue * const vals = A->values;
    sptValue * buffer_lambda;
    int nthreads = 1;

#ifdef PARTI_USE_OPENMP
    #pragma omp parallel for schedule(static)
#endif
    for(sptElementIndex j=0; j < ncols; ++j) {
        lambda[j] = 0.0;
    }

#ifdef PARTI_USE_OPENMP
    #pragma omp parallel
    {
        nthreads = omp_get_num_threads();
    }
    buffer_lambda = (sptValue *)malloc(nthreads * ncols * sizeof(sptValue));
#endif


#ifdef PARTI_USE_OPENMP
    #pragma omp parallel
    {
        #pragma omp for schedule(static)
        for(sptNnzIndex j=0; j < (sptNnzIndex)nthreads * ncols; ++j)
            buffer_lambda[j] = 0.0;

        int const tid = omp_get_thread_num();
        int const nthreads = omp_get_num_threads();
        sptValue * loc_lambda = buffer_lambda + tid * ncols;

        #pragma omp for
        for(sptIndex i=0; i < nrows; ++i) {
            for(sptElementIndex j=0; j < ncols; ++j) {
                if(vals[i*stride + j] > loc_lambda[j])
                    loc_lambda[j] = vals[i*stride + j];
            }
        }

        #pragma omp for schedule(static)
        for(sptElementIndex j=0; j < ncols; ++j) {
            for(sptIndex i=0; i < (sptIndex)nthreads; ++i) {
                if(buffer_lambda[i*ncols + j] > lambda[j])
                    lambda[j] = buffer_lambda[i*ncols + j];
            }
        }
    }   /* end parallel pragma */

#else
    for(sptIndex i=0; i < nrows; ++i) {
        for(sptElementIndex j=0; j < ncols; ++j) {
            if(vals[i*stride + j] > lambda[j])
                lambda[j] = vals[i*stride + j];
        }
    }
#endif

#ifdef PARTI_USE_OPENMP
        #pragma omp parallel for schedule(static)
#endif
        for(sptElementIndex j=0; j < ncols; ++j) {
            if(lambda[j] < 1)
                lambda[j] = 1;
        }

#ifdef PARTI_USE_OPENMP
        #pragma omp parallel for
#endif
        for(sptIndex i=0; i < nrows; ++i) {
            for(sptElementIndex j=0; j < ncols; ++j) {
                vals[i*stride + j] /= lambda[j];
            }
        }

#ifdef PARTI_USE_OPENMP
    free(buffer_lambda);
#endif

    return 0;
}


void GetRankFinalLambda(
  sptElementIndex const rank,
  sptIndex const nmodes,
  sptRankMatrix ** mats,
  sptValue * const lambda)
{
  sptValue * tmp_lambda =  (sptValue *) malloc(rank * sizeof(*tmp_lambda));

  for(sptIndex m=0; m < nmodes; ++m) {   
    sptRankMatrix2Norm(mats[m], tmp_lambda);
    for(sptElementIndex r=0; r < rank; ++r) {
      lambda[r] *= tmp_lambda[r];
    }
  }

  free(tmp_lambda);
}


