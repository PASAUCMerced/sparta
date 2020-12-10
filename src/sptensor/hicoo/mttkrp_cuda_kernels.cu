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
#include "hicoo.h"
#include "mttkrp_cuda_kernels.h"
#include <inttypes.h>

int sptMTTKRPKernelHiCOO(
    const sptIndex mode,
    const sptIndex nmodes,
    const sptNnzIndex nnz,
    const sptNnzIndex max_nnzb,
    const sptIndex R,
    const sptIndex stride,
    const sptElementIndex sb_bits,
    const sptElementIndex sc_bits,
    const sptIndex blength,
    const int impl_num,
    const sptNnzIndex kptr_begin,
    const sptNnzIndex kptr_end,
    sptIndex * const dev_ndims,
    sptNnzIndex * const dev_cptr,
    sptNnzIndex * const dev_bptr,
    sptBlockIndex ** const dev_binds,
    sptElementIndex ** const dev_einds,
    sptValue * const dev_values,
    sptIndex * const dev_mats_order,
    sptValue ** const dev_mats)
{
    int result = 0;

    /* Maximum settings */
    sptIndex max_nthreads_per_block = 256;
    sptIndex max_nblocks = 32768;
    sptIndex max_R = 4;

    sptIndex nthreadsx = 0;
    sptIndex nthreadsy = 0;
    sptIndex nblocks = 0;
    sptIndex shr_size = 0;
    sptNnzIndex all_nblocks = blength;

    switch(nmodes) {
    case 3: /* 3-D tensors */
        switch(impl_num) {
        case 1: // Naive, 1D
            /* Set number of blocks and threads */
            nthreadsx = max_nnzb;
            if(all_nblocks < max_nblocks) {
                nblocks = all_nblocks;
            } else {
                nblocks = max_nblocks;
            }
            // shr_size = 2 * nmodes * sizeof(sptIndex);
            break;
        case 2:
            nthreadsy = R;
            nthreadsx = max_nnzb;
            if(all_nblocks < max_nblocks) {
                nblocks = all_nblocks;
            } else {
                nblocks = max_nblocks;
            }
            // shr_size = 2 * nmodes * sizeof(sptIndex);
            break;
        case 3:
            nthreadsx = R;
            nthreadsy = max_nnzb;
            if(all_nblocks < max_nblocks) {
                nblocks = all_nblocks;
            } else {
                nblocks = max_nblocks;
            }
            // shr_size = 2 * nmodes * sizeof(sptIndex);
            break;
        case 4:
            nthreadsx = R;
            if(R <= max_R)
                nthreadsx = R;
            else
                nthreadsx = max_R;
            nthreadsy = max_nnzb;
            if(all_nblocks < max_nblocks) {
                nblocks = all_nblocks;
            } else {
                nblocks = max_nblocks;
            }
            // shr_size = 2 * nmodes * sizeof(sptIndex);
            break;

        /* Matrix blocked implementations */
        case 14:
            nthreadsx = R;
            if(R <= max_R)
                nthreadsx = R;
            else
                nthreadsx = max_R;
            nthreadsy = max_nnzb;
            if(all_nblocks < max_nblocks) {
                nblocks = all_nblocks;
            } else {
                nblocks = max_nblocks;
            }
            // shr_size = 2 * nmodes * sizeof(sptIndex);
            break;

        /* Shared memory for the output matrix + switch block sizes */
        case 15:
            nthreadsx = R;
            if(R <= max_R) {
                nthreadsx = R;
            }
            else {
                nthreadsx = max_R;
            }
            nthreadsy = max_nnzb;
            if(all_nblocks < max_nblocks) {
                nblocks = all_nblocks;
            } else {
                nblocks = max_nblocks;
            }
            shr_size = (sptIndex)pow(2, sb_bits) * R * sizeof(sptValue);
            break;

        /* Shared memory for three matrices + switch block sizes */
        case 16:
            nthreadsx = R;
            if(R <= max_R) {
                nthreadsx = R;
            }
            else {
                nthreadsx = max_R;
            }
            nthreadsy = max_nnzb;
            if(all_nblocks < max_nblocks) {
                nblocks = all_nblocks;
            } else {
                nblocks = max_nblocks;
            }
            shr_size = nmodes * (sptIndex)pow(2, sb_bits) * R * sizeof(sptValue);
            break;

        }

        dim3 dimBlock(nthreadsx, nthreadsy);
        switch(impl_num) {
        case 1: // Naive, 1D
            printf("\nExecute spt_MTTKRPKernelHiCOO_3D_naive (%u, %u)\n", nblocks, nthreadsx);
            spt_MTTKRPKernelHiCOO_3D_naive<<<nblocks, nthreadsx>>>(
                mode,
                nmodes,
                nnz,
                R,
                stride,
                sb_bits,
                sc_bits,
                blength,
                kptr_begin,
                kptr_end,
                dev_ndims,
                dev_cptr,
                dev_bptr,
                dev_binds,
                dev_einds,
                dev_values,
                dev_mats_order,
                dev_mats);
            break;
        case 2:
            printf("\nExecute spt_MTTKRPKernelRankHiCOO_3D_naive (%u, %u, %u)\n", nblocks, nthreadsx, nthreadsy);

            spt_MTTKRPKernelRankHiCOO_3D_naive<<<nblocks, dimBlock>>>(
                mode,
                nmodes,
                nnz,
                R,
                stride,
                sb_bits,
                sc_bits,
                blength,
                kptr_begin,
                kptr_end,
                dev_ndims,
                dev_cptr,
                dev_bptr,
                dev_binds,
                dev_einds,
                dev_values,
                dev_mats_order,
                dev_mats);
            break;
        case 3:
            printf("\nExecute spt_MTTKRPKernelRankSplitHiCOO_3D_naive (%u, %u, %u)\n", nblocks, nthreadsx, nthreadsy);

            spt_MTTKRPKernelRankSplitHiCOO_3D_naive<<<nblocks, dimBlock>>>(
                mode,
                nmodes,
                nnz,
                R,
                stride,
                sb_bits,
                sc_bits,
                blength,
                kptr_begin,
                kptr_end,
                dev_ndims,
                dev_cptr,
                dev_bptr,
                dev_binds,
                dev_einds,
                dev_values,
                dev_mats_order,
                dev_mats);
            break;
        case 4:
            printf("\nExecute spt_MTTKRPKernelRankSplitHiCOORB_3D_naive (%u, %u, %u)\n", nblocks, nthreadsx, nthreadsy);

            spt_MTTKRPKernelRankSplitHiCOORB_3D_naive<<<nblocks, dimBlock>>>(
                mode,
                nmodes,
                nnz,
                R,
                stride,
                sb_bits,
                sc_bits,
                blength,
                kptr_begin,
                kptr_end,
                dev_ndims,
                dev_cptr,
                dev_bptr,
                dev_binds,
                dev_einds,
                dev_values,
                dev_mats_order,
                dev_mats);
            break;

        /* Matrix blocked implementations */
        case 14:
            printf("\nExecute spt_MTTKRPKernelRankSplitHiCOORB_3D_MatrixBlocked (%u, %u, %u)\n", nblocks, nthreadsx, nthreadsy);

            spt_MTTKRPKernelRankSplitHiCOORB_3D_MatrixBlocked<<<nblocks, dimBlock>>>(
                mode,
                nmodes,
                nnz,
                R,
                stride,
                sb_bits,
                sc_bits,
                blength,
                kptr_begin,
                kptr_end,
                dev_ndims,
                dev_cptr,
                dev_bptr,
                dev_binds,
                dev_einds,
                dev_values,
                dev_mats_order,
                dev_mats);
            break;

        /* Shared memory for the output matrix + switch block sizes */
        case 15:
            printf("\nExecute spt_MTTKRPKernelRankSplitHiCOORB_3D_MatrixBlocked_SM (%u, %u, %u), SM: %u bytes\n", nblocks, nthreadsx, nthreadsy, shr_size);

            spt_MTTKRPKernelRankSplitHiCOORB_3D_MatrixBlocked_SM<<<nblocks, dimBlock, shr_size>>>(
                mode,
                nmodes,
                nnz,
                R,
                stride,
                sb_bits,
                sc_bits,
                blength,
                kptr_begin,
                kptr_end,
                dev_ndims,
                dev_cptr,
                dev_bptr,
                dev_binds,
                dev_einds,
                dev_values,
                dev_mats_order,
                dev_mats);
            break;

        /* Shared memory for three matrices + switch block sizes */
        case 16:
            printf("\nExecute spt_MTTKRPKernelRankSplitHiCOORB_3D_MatrixBlocked_AllSM (%u, %u, %u), SM: %u bytes\n", nblocks, nthreadsx, nthreadsy, shr_size);

            spt_MTTKRPKernelRankSplitHiCOORB_3D_MatrixBlocked_AllSM<<<nblocks, dimBlock, shr_size>>>(
                mode,
                nmodes,
                nnz,
                R,
                stride,
                sb_bits,
                sc_bits,
                blength,
                kptr_begin,
                kptr_end,
                dev_ndims,
                dev_cptr,
                dev_bptr,
                dev_binds,
                dev_einds,
                dev_values,
                dev_mats_order,
                dev_mats);
            break;

         }

    break;
    }   // End switch nmodes
    result = cudaThreadSynchronize();
    spt_CheckCudaError(result != 0, "CUDA HiCOO SpTns MTTKRP");

    return 0;
}


/* impl_num = 01  Naive, 1-D 
 * Limitation: blockDim.x (max_nnz) <= 1024.
 */
__global__ void spt_MTTKRPKernelHiCOO_3D_naive(
    const sptIndex mode,
    const sptIndex nmodes,
    const sptNnzIndex nnz,
    const sptIndex R,
    const sptIndex stride,
    const sptElementIndex sb_bits,
    const sptElementIndex sc_bits,
    const sptNnzIndex blength,
    const sptNnzIndex kptr_begin,
    const sptNnzIndex kptr_end,
    sptIndex * const dev_ndims,
    sptNnzIndex * const dev_cptr,
    sptNnzIndex * const dev_bptr,
    sptBlockIndex ** const dev_binds,
    sptElementIndex ** const dev_einds,
    sptValue * const dev_values,
    sptIndex * const dev_mats_order,
    sptValue ** const dev_mats)
{
    sptNnzIndex const all_nblocks = blength;
    const sptIndex tidx = threadIdx.x;
    sptNnzIndex z;
    sptIndex block_coord_mode, block_coord_1, block_coord_2;

    sptValue * const mvals = dev_mats[nmodes];
    sptIndex const times_mat_index_1 = dev_mats_order[1];
    sptValue * const times_mat_1 = dev_mats[times_mat_index_1];
    sptIndex const times_mat_index_2 = dev_mats_order[2];
    sptValue * const times_mat_2 = dev_mats[times_mat_index_2];

    sptNnzIndex num_loops_blocks = 1;
    if(all_nblocks > gridDim.x) {
        num_loops_blocks = (all_nblocks + gridDim.x - 1) / gridDim.x;
    }
    for(sptNnzIndex nb=0; nb<num_loops_blocks; ++nb) {
        /* Block level */
        sptNnzIndex b = blockIdx.x + nb * gridDim.x;
        if(b < blength) {
            /* Block indices */
            block_coord_mode = dev_binds[mode][b] << sb_bits;
            block_coord_1 = dev_binds[times_mat_index_1][b] << sb_bits;
            block_coord_2 = dev_binds[times_mat_index_2][b] << sb_bits;

            /* TODO: duplicated in registers */
            sptNnzIndex const bptr_begin = dev_bptr[b];
            sptNnzIndex const bptr_end = dev_bptr[b+1];

            /* Thread level */
            z = tidx + bptr_begin;
            if(z < bptr_end) {
                sptValue const entry = dev_values[z];
                sptNnzIndex const mode_i = block_coord_mode + dev_einds[mode][z];
                sptNnzIndex const tmp_i_1 = block_coord_1 + dev_einds[times_mat_index_1][z];
                sptNnzIndex const tmp_i_2 = block_coord_2 + dev_einds[times_mat_index_2][z];

                sptValue tmp_val = 0;
                for(sptIndex r=0; r<R; ++r) {
                    tmp_val = entry * times_mat_1[tmp_i_1 * stride + r] * times_mat_2[tmp_i_2 * stride + r];
                    atomicAdd(&(mvals[mode_i * stride + r]), tmp_val);
                }

            }   // End loop entries
        }
    }   // End loop blocks

}

/* impl_num = 02  Naive, 2-D 
 * Limitation: blockDim.x (max_nnz) * R <= 1024.
 */
__global__ void spt_MTTKRPKernelRankHiCOO_3D_naive(
    const sptIndex mode,
    const sptIndex nmodes,
    const sptNnzIndex nnz,
    const sptIndex R,
    const sptIndex stride,
    const sptElementIndex sb_bits,
    const sptElementIndex sc_bits,
    const sptNnzIndex blength,
    const sptNnzIndex kptr_begin,
    const sptNnzIndex kptr_end,
    sptIndex * const dev_ndims,
    sptNnzIndex * const dev_cptr,
    sptNnzIndex * const dev_bptr,
    sptBlockIndex ** const dev_binds,
    sptElementIndex ** const dev_einds,
    sptValue * const dev_values,
    sptIndex * const dev_mats_order,
    sptValue ** const dev_mats)
{
    sptNnzIndex const all_nblocks = blength;
    const sptIndex tidx = threadIdx.x;
    const sptIndex tidy = threadIdx.y;
    sptNnzIndex z;
    sptIndex block_coord_mode, block_coord_1, block_coord_2;

    sptValue * const mvals = dev_mats[nmodes];
    sptIndex const times_mat_index_1 = dev_mats_order[1];
    sptValue * const times_mat_1 = dev_mats[times_mat_index_1];
    sptIndex const times_mat_index_2 = dev_mats_order[2];
    sptValue * const times_mat_2 = dev_mats[times_mat_index_2];

    sptNnzIndex num_loops_blocks = 1;
    if(all_nblocks > gridDim.x) {
        num_loops_blocks = (all_nblocks + gridDim.x - 1) / gridDim.x;
    }
    for(sptNnzIndex nb=0; nb<num_loops_blocks; ++nb) {
        /* Block level */
        sptNnzIndex b = blockIdx.x + nb * gridDim.x;
        if(b < blength) {
            /* Block indices */
            block_coord_mode = dev_binds[mode][b] << sb_bits;
            block_coord_1 = dev_binds[times_mat_index_1][b] << sb_bits;
            block_coord_2 = dev_binds[times_mat_index_2][b] << sb_bits;

            /* TODO: duplicated in registers */
            sptNnzIndex const bptr_begin = dev_bptr[b];
            sptNnzIndex const bptr_end = dev_bptr[b+1];

            /* Thread level */
            z = tidx + bptr_begin;
            if(z < bptr_end) {
                /* TODO: duplicated in R threads */
                sptValue const entry = dev_values[z];
                sptNnzIndex const mode_i = block_coord_mode + dev_einds[mode][z];
                sptNnzIndex const tmp_i_1 = block_coord_1 + dev_einds[times_mat_index_1][z];
                sptNnzIndex const tmp_i_2 = block_coord_2 + dev_einds[times_mat_index_2][z];

                sptValue tmp_val = 0;
                tmp_val = entry * times_mat_1[tmp_i_1 * stride + tidy] * times_mat_2[tmp_i_2 * stride + tidy];
                atomicAdd(&(mvals[mode_i * stride + tidy]), tmp_val);

            }   // End loop entries
        }
    }   // End loop blocks

}

/* impl_num = 03  Naive, 2-D, exchange tidx and tidy.
 * Limitation: R * blockDim.y (max_nnz) <= 1024.
 */
__global__ void spt_MTTKRPKernelRankSplitHiCOO_3D_naive(
    const sptIndex mode,
    const sptIndex nmodes,
    const sptNnzIndex nnz,
    const sptIndex R,
    const sptIndex stride,
    const sptElementIndex sb_bits,
    const sptElementIndex sc_bits,
    const sptNnzIndex blength,
    const sptNnzIndex kptr_begin,
    const sptNnzIndex kptr_end,
    sptIndex * const dev_ndims,
    sptNnzIndex * const dev_cptr,
    sptNnzIndex * const dev_bptr,
    sptBlockIndex ** const dev_binds,
    sptElementIndex ** const dev_einds,
    sptValue * const dev_values,
    sptIndex * const dev_mats_order,
    sptValue ** const dev_mats)
{
    sptNnzIndex const all_nblocks = blength;
    const sptIndex tidx = threadIdx.x;
    const sptIndex tidy = threadIdx.y;
    sptNnzIndex z;
    sptIndex block_coord_mode, block_coord_1, block_coord_2;

    sptValue * const mvals = dev_mats[nmodes];
    sptIndex const times_mat_index_1 = dev_mats_order[1];
    sptValue * const times_mat_1 = dev_mats[times_mat_index_1];
    sptIndex const times_mat_index_2 = dev_mats_order[2];
    sptValue * const times_mat_2 = dev_mats[times_mat_index_2];

    sptNnzIndex num_loops_blocks = 1;
    if(all_nblocks > gridDim.x) {
        num_loops_blocks = (all_nblocks + gridDim.x - 1) / gridDim.x;
    }
    for(sptNnzIndex nb=0; nb<num_loops_blocks; ++nb) {
        /* Block level */
        sptNnzIndex b = blockIdx.x + nb * gridDim.x;
        if(b < blength) {
            /* Block indices */
            block_coord_mode = dev_binds[mode][b] << sb_bits;
            block_coord_1 = dev_binds[times_mat_index_1][b] << sb_bits;
            block_coord_2 = dev_binds[times_mat_index_2][b] << sb_bits;

            /* TODO: duplicated in registers */
            sptNnzIndex const bptr_begin = dev_bptr[b];
            sptNnzIndex const bptr_end = dev_bptr[b+1];

            /* Thread level */
            z = tidy + bptr_begin;
            if(z < bptr_end) {
                sptValue const entry = dev_values[z];
                sptNnzIndex const mode_i = block_coord_mode + dev_einds[mode][z];
                sptNnzIndex const tmp_i_1 = block_coord_1 + dev_einds[times_mat_index_1][z];
                sptNnzIndex const tmp_i_2 = block_coord_2 + dev_einds[times_mat_index_2][z];

                sptValue tmp_val = 0;
                tmp_val = entry * times_mat_1[tmp_i_1 * stride + tidx] * times_mat_2[tmp_i_2 * stride + tidx];
                atomicAdd(&(mvals[mode_i * stride + tidx]), tmp_val);

            }   // End loop entries
        }
    }   // End loop blocks

}

/* impl_num = 04  Naive, 2-D, with rank blocking.
 * Limitation: max_R * blockDim.y (max_nnz) <= 1024.
 */
__global__ void spt_MTTKRPKernelRankSplitHiCOORB_3D_naive(
    const sptIndex mode,
    const sptIndex nmodes,
    const sptNnzIndex nnz,
    const sptIndex R,
    const sptIndex stride,
    const sptElementIndex sb_bits,
    const sptElementIndex sc_bits,
    const sptNnzIndex blength,
    const sptNnzIndex kptr_begin,
    const sptNnzIndex kptr_end,
    sptIndex * const dev_ndims,
    sptNnzIndex * const dev_cptr,
    sptNnzIndex * const dev_bptr,
    sptBlockIndex ** const dev_binds,
    sptElementIndex ** const dev_einds,
    sptValue * const dev_values,
    sptIndex * const dev_mats_order,
    sptValue ** const dev_mats)
{
    sptNnzIndex const all_nblocks = blength;
    const sptIndex tidx = threadIdx.x;
    const sptIndex tidy = threadIdx.y;
    sptNnzIndex z;
    sptIndex block_coord_mode, block_coord_1, block_coord_2;
    const sptIndex num_loops_r = R / blockDim.x;
    const sptIndex rest_loop = R - num_loops_r * blockDim.x;

    sptValue * const mvals = dev_mats[nmodes];
    sptIndex const times_mat_index_1 = dev_mats_order[1];
    sptValue * const times_mat_1 = dev_mats[times_mat_index_1];
    sptIndex const times_mat_index_2 = dev_mats_order[2];
    sptValue * const times_mat_2 = dev_mats[times_mat_index_2];

    sptNnzIndex num_loops_blocks = 1;
    if(all_nblocks > gridDim.x) {
        num_loops_blocks = (all_nblocks + gridDim.x - 1) / gridDim.x;
    }

    for(sptNnzIndex nb=0; nb<num_loops_blocks; ++nb) {
        /* Block level */
        sptNnzIndex b = blockIdx.x + nb * gridDim.x;
        if(b < blength) {
            /* Block indices */
            block_coord_mode = dev_binds[mode][b] << sb_bits;
            block_coord_1 = dev_binds[times_mat_index_1][b] << sb_bits;
            block_coord_2 = dev_binds[times_mat_index_2][b] << sb_bits;

            /* TODO: duplicated in registers */
            sptNnzIndex const bptr_begin = dev_bptr[b];
            sptNnzIndex const bptr_end = dev_bptr[b+1];

            /* Thread level */
            z = tidy + bptr_begin;
            if(z < bptr_end) {
                sptValue const entry = dev_values[z];
                sptNnzIndex const mode_i = block_coord_mode + dev_einds[mode][z];
                sptNnzIndex const tmp_i_1 = block_coord_1 + dev_einds[times_mat_index_1][z];
                sptNnzIndex const tmp_i_2 = block_coord_2 + dev_einds[times_mat_index_2][z];

                sptIndex r;
                sptValue tmp_val = 0;
                for(sptIndex l=0; l<num_loops_r; ++l) {
                    r = tidx + l * blockDim.x;
                    tmp_val = entry * times_mat_1[tmp_i_1 * stride + r] * times_mat_2[tmp_i_2 * stride + r];
                    atomicAdd(&(mvals[mode_i * stride + r]), tmp_val);
                }

                if(rest_loop > 0 && tidx < rest_loop) {
                    r = tidx + num_loops_r * blockDim.x;
                    tmp_val = entry * times_mat_1[tmp_i_1 * stride + r] * times_mat_2[tmp_i_2 * stride + r];
                    atomicAdd(&(mvals[mode_i * stride + r]), tmp_val);
                }

            }   // End loop entries
        }
    }   // End loop blocks

}



/* impl_num = 14  Matrix Blocked, 2-D, with rank blocking.
 * Limitation: max_R * blockDim.y (max_nnz) <= 1024.
 */
__global__ void spt_MTTKRPKernelRankSplitHiCOORB_3D_MatrixBlocked(
    const sptIndex mode,
    const sptIndex nmodes,
    const sptNnzIndex nnz,
    const sptIndex R,
    const sptIndex stride,
    const sptElementIndex sb_bits,
    const sptElementIndex sc_bits,
    const sptNnzIndex blength,
    const sptNnzIndex kptr_begin,
    const sptNnzIndex kptr_end,
    sptIndex * const dev_ndims,
    sptNnzIndex * const dev_cptr,
    sptNnzIndex * const dev_bptr,
    sptBlockIndex ** const dev_binds,
    sptElementIndex ** const dev_einds,
    sptValue * const dev_values,
    sptIndex * const dev_mats_order,
    sptValue ** const dev_mats)
{
    sptNnzIndex const all_nblocks = blength;
    const sptIndex tidx = threadIdx.x;
    const sptIndex tidy = threadIdx.y;
    sptNnzIndex z;
    const sptIndex num_loops_r = R / blockDim.x;
    const sptIndex rest_loop = R - num_loops_r * blockDim.x;

    sptValue * const mvals = dev_mats[nmodes];
    sptIndex const times_mat_index_1 = dev_mats_order[1];
    sptValue * const times_mat_1 = dev_mats[times_mat_index_1];
    sptIndex const times_mat_index_2 = dev_mats_order[2];
    sptValue * const times_mat_2 = dev_mats[times_mat_index_2];

    sptNnzIndex num_loops_blocks = 1;
    if(all_nblocks > gridDim.x) {
        num_loops_blocks = (all_nblocks + gridDim.x - 1) / gridDim.x;
    }

    for(sptNnzIndex nb=0; nb<num_loops_blocks; ++nb) {
        /* Block level */
        sptNnzIndex b = blockIdx.x + nb * gridDim.x;
        if(b < blength) {
            /* TODO: duplicated in registers */
            sptValue * blocked_mvals = mvals + (dev_binds[mode][b] << sb_bits) * stride;
            sptValue * blocked_times_mat_1 = times_mat_1 + (dev_binds[times_mat_index_1][b] << sb_bits) * stride;
            sptValue * blocked_times_mat_2 = times_mat_2 + (dev_binds[times_mat_index_2][b] << sb_bits) * stride;

            sptNnzIndex const bptr_begin = dev_bptr[b];
            sptNnzIndex const bptr_end = dev_bptr[b+1];

            /* Thread level */
            z = tidy + bptr_begin;
            if(z < bptr_end) {
                sptValue const entry = dev_values[z];
                sptElementIndex const mode_i = dev_einds[mode][z];
                sptElementIndex const tmp_i_1 = dev_einds[times_mat_index_1][z];
                sptElementIndex const tmp_i_2 = dev_einds[times_mat_index_2][z];

                sptValue * const bmvals_row = blocked_mvals + mode_i * stride;

                sptIndex r;
                sptValue tmp_val = 0;
                for(sptIndex l=0; l<num_loops_r; ++l) {
                    r = tidx + l * blockDim.x;
                    tmp_val = entry * blocked_times_mat_1[tmp_i_1 * stride + r] * blocked_times_mat_2[tmp_i_2 * stride + r];
                    atomicAdd(&(bmvals_row[r]), tmp_val);
                }

                if(rest_loop > 0 && tidx < rest_loop) {
                    r = tidx + num_loops_r * blockDim.x;
                    tmp_val = entry * blocked_times_mat_1[tmp_i_1 * stride + r] * blocked_times_mat_2[tmp_i_2 * stride + r];
                    atomicAdd(&(bmvals_row[r]), tmp_val);
                }

            }   // End loop entries
        }
    }   // End loop blocks

}



/* impl_num = 15  Matrix Blocked, 2-D, with rank blocking.
 * + switch according to block size
 * use shared memory for the output matrix
 * Limitation: max_R * blockDim.y (max_nnz) <= 1024.
 */
__global__ void spt_MTTKRPKernelRankSplitHiCOORB_3D_MatrixBlocked_SM(
    const sptIndex mode,
    const sptIndex nmodes,
    const sptNnzIndex nnz,
    const sptIndex R,
    const sptIndex stride,
    const sptElementIndex sb_bits,
    const sptElementIndex sc_bits,
    const sptNnzIndex blength,
    const sptNnzIndex kptr_begin,
    const sptNnzIndex kptr_end,
    sptIndex * const dev_ndims,
    sptNnzIndex * const dev_cptr,
    sptNnzIndex * const dev_bptr,
    sptBlockIndex ** const dev_binds,
    sptElementIndex ** const dev_einds,
    sptValue * const dev_values,
    sptIndex * const dev_mats_order,
    sptValue ** const dev_mats)
{
    sptIndex const sb_size = (sptIndex) 1 << sb_bits;
    /* Data in shared memory */
    extern __shared__ sptValue mempool[];
    sptValue * sm_blocked_mvals = mempool;

    sptNnzIndex const all_nblocks = blength;
    const sptIndex tidx = threadIdx.x;
    const sptIndex tidy = threadIdx.y;
    sptNnzIndex z;
    const sptIndex num_loops_r = R / blockDim.x;
    const sptIndex rest_loop = R - num_loops_r * blockDim.x;
    sptIndex r;

    sptValue * const mvals = dev_mats[nmodes];
    sptIndex const times_mat_index_1 = dev_mats_order[1];
    sptValue * const times_mat_1 = dev_mats[times_mat_index_1];
    sptIndex const times_mat_index_2 = dev_mats_order[2];
    sptValue * const times_mat_2 = dev_mats[times_mat_index_2];

    sptNnzIndex num_loops_blocks = 1;
    if(all_nblocks > gridDim.x) {
        num_loops_blocks = (all_nblocks + gridDim.x - 1) / gridDim.x;
    }

    for(sptNnzIndex nb=0; nb<num_loops_blocks; ++nb) {
        /* Block level */
        sptNnzIndex b = blockIdx.x + nb * gridDim.x;
        if(b < blength) {
            /* TODO: duplicated in registers */
            sptIndex blocked_mode_i = dev_binds[mode][b] << sb_bits;
            sptValue * blocked_mvals = mvals + (dev_binds[mode][b] << sb_bits) * stride;
            sptValue * blocked_times_mat_1 = times_mat_1 + (dev_binds[times_mat_index_1][b] << sb_bits) * stride;
            sptValue * blocked_times_mat_2 = times_mat_2 + (dev_binds[times_mat_index_2][b] << sb_bits) * stride;

            sptNnzIndex const bptr_begin = dev_bptr[b];
            sptNnzIndex const bptr_end = dev_bptr[b+1];

            /* Enough matrix reuse */
            if (bptr_end - bptr_begin > sb_size) {

                /* Load mats[nmodes] into shared memory, use R instead of stride. */
                if (tidy < sb_size) {
                    for(sptIndex l=0; l<num_loops_r; ++l) {
                        r = tidx + l * blockDim.x;
                        sm_blocked_mvals[tidy * R + r] = 0;
                    }
                    if(rest_loop > 0 && tidx < rest_loop) {
                        r = tidx + num_loops_r * blockDim.x;
                        sm_blocked_mvals[tidy * R + r] = 0;
                    }
                }
                __syncthreads();

                /* Thread level */
                z = tidy + bptr_begin;
                if(z < bptr_end) {
                    sptValue const entry = dev_values[z];
                    sptElementIndex const mode_i = dev_einds[mode][z];
                    sptElementIndex const tmp_i_1 = dev_einds[times_mat_index_1][z];
                    sptElementIndex const tmp_i_2 = dev_einds[times_mat_index_2][z];

                    sptValue * const bmvals_row = sm_blocked_mvals + mode_i * R;
                    sptValue * const blocked_times_mat_1_row = blocked_times_mat_1 + tmp_i_1 * stride;
                    sptValue * const blocked_times_mat_2_row = blocked_times_mat_2 + tmp_i_2 * stride;

                    sptValue tmp_val = 0;
                    for(sptIndex l=0; l<num_loops_r; ++l) {
                        r = tidx + l * blockDim.x;
                        tmp_val = entry * blocked_times_mat_1_row[r] * blocked_times_mat_2_row[r];
                        atomicAdd(&(bmvals_row[r]), tmp_val);
                    }

                    if(rest_loop > 0 && tidx < rest_loop) {
                        r = tidx + num_loops_r * blockDim.x;
                        tmp_val = entry * blocked_times_mat_1_row[r] * blocked_times_mat_2_row[r];
                        atomicAdd(&(bmvals_row[r]), tmp_val);
                    }

                }   // End loop entries

                /* Store back mats[nmodes] from shared memory */
                if (tidy < sb_size) {
                    for(sptIndex l=0; l<num_loops_r; ++l) {
                        r = tidx + l * blockDim.x;
                        atomicAdd( &(blocked_mvals[tidy * stride + r]),  sm_blocked_mvals[tidy * stride + r] );
                    }
                    if(rest_loop > 0 && tidx < rest_loop) {
                        r = tidx + num_loops_r * blockDim.x;
                        atomicAdd( &(blocked_mvals[tidy * stride + r]),  sm_blocked_mvals[tidy * stride + r] );
                    }
                }


            } else { /* Not enough matrix reuse */
                /* Thread level */
                z = tidy + bptr_begin;
                if(z < bptr_end) {
                    sptValue const entry = dev_values[z];
                    sptElementIndex const mode_i = dev_einds[mode][z];
                    sptElementIndex const tmp_i_1 = dev_einds[times_mat_index_1][z];
                    sptElementIndex const tmp_i_2 = dev_einds[times_mat_index_2][z];

                    sptValue * const bmvals_row = blocked_mvals + mode_i * stride;

                    sptValue tmp_val = 0;
                    for(sptIndex l=0; l<num_loops_r; ++l) {
                        r = tidx + l * blockDim.x;
                        tmp_val = entry * blocked_times_mat_1[tmp_i_1 * stride + r] * blocked_times_mat_2[tmp_i_2 * stride + r];
                        atomicAdd(&(bmvals_row[r]), tmp_val);
                    }

                    if(rest_loop > 0 && tidx < rest_loop) {
                        r = tidx + num_loops_r * blockDim.x;
                        tmp_val = entry * blocked_times_mat_1[tmp_i_1 * stride + r] * blocked_times_mat_2[tmp_i_2 * stride + r];
                        atomicAdd(&(bmvals_row[r]), tmp_val);
                    }

                }   // End loop entries
            }   // End if: block size

        }   // End if: block range
    }   // End loop blocks

}



/* impl_num = 16  Matrix Blocked, 2-D, with rank blocking. TODO: BUG EXISTS.
 * + switch according to block size
 * use shared memory for three matrices
 * Limitation: max_R * blockDim.y (max_nnz) <= 1024.
 */
__global__ void spt_MTTKRPKernelRankSplitHiCOORB_3D_MatrixBlocked_AllSM(
    const sptIndex mode,
    const sptIndex nmodes,
    const sptNnzIndex nnz,
    const sptIndex R,
    const sptIndex stride,
    const sptElementIndex sb_bits,
    const sptElementIndex sc_bits,
    const sptNnzIndex blength,
    const sptNnzIndex kptr_begin,
    const sptNnzIndex kptr_end,
    sptIndex * const dev_ndims,
    sptNnzIndex * const dev_cptr,
    sptNnzIndex * const dev_bptr,
    sptBlockIndex ** const dev_binds,
    sptElementIndex ** const dev_einds,
    sptValue * const dev_values,
    sptIndex * const dev_mats_order,
    sptValue ** const dev_mats)
{
    sptIndex const sb_size = (sptIndex)powf(2, sb_bits);
    /* Data in shared memory */
    extern __shared__ sptValue mempool[];
    sptValue * sm_blocked_mvals = mempool;
    sptValue * sm_blocked_times_mat_1 = mempool + sb_size * R * sizeof(sptValue);
    sptValue * sm_blocked_times_mat_2 = sm_blocked_times_mat_1 + sb_size * R * sizeof(sptValue);

    sptNnzIndex const all_nblocks = blength;
    const sptIndex tidx = threadIdx.x;
    const sptIndex tidy = threadIdx.y;
    sptNnzIndex z;
    const sptIndex num_loops_r = R / blockDim.x;
    const sptIndex rest_loop = R - num_loops_r * blockDim.x;
    sptIndex r;

    sptValue * const mvals = dev_mats[nmodes];
    sptIndex const times_mat_index_1 = dev_mats_order[1];
    sptValue * const times_mat_1 = dev_mats[times_mat_index_1];
    sptIndex const times_mat_index_2 = dev_mats_order[2];
    sptValue * const times_mat_2 = dev_mats[times_mat_index_2];

    sptNnzIndex num_loops_blocks = 1;
    if(all_nblocks > gridDim.x) {
        num_loops_blocks = (all_nblocks + gridDim.x - 1) / gridDim.x;
    }

    for(sptNnzIndex nb=0; nb<num_loops_blocks; ++nb) {
        /* Block level */
        sptNnzIndex b = blockIdx.x + nb * gridDim.x;
        if(b < blength) {
            /* TODO: duplicated in registers */
            sptIndex blocked_mode_i = dev_binds[mode][b] << sb_bits;
            sptValue * blocked_mvals = mvals + (dev_binds[mode][b] << sb_bits) * stride;
            sptValue * blocked_times_mat_1 = times_mat_1 + (dev_binds[times_mat_index_1][b] << sb_bits) * stride;
            sptValue * blocked_times_mat_2 = times_mat_2 + (dev_binds[times_mat_index_2][b] << sb_bits) * stride;

            sptNnzIndex const bptr_begin = dev_bptr[b];
            sptNnzIndex const bptr_end = dev_bptr[b+1];

            /* Enough matrix reuse */
            if (bptr_end - bptr_begin > sb_size) {

                /* Load mats[nmodes] into shared memory, use R instead of stride. */
                if (tidy < sb_size) {
                    for(sptIndex l=0; l<num_loops_r; ++l) {
                        r = tidx + l * blockDim.x;
                        sm_blocked_mvals[tidy * R + r] = 0;
                        sm_blocked_times_mat_1[tidy * R + r] = blocked_times_mat_1[tidy * stride + r];
                        sm_blocked_times_mat_2[tidy * R + r] = blocked_times_mat_2[tidy * stride + r];
                    }
                    if(rest_loop > 0 && tidx < rest_loop) {
                        r = tidx + num_loops_r * blockDim.x;
                        sm_blocked_mvals[tidy * R + r] = 0;
                        sm_blocked_times_mat_1[tidy * R + r] = blocked_times_mat_1[tidy * stride + r];
                        sm_blocked_times_mat_2[tidy * R + r] = blocked_times_mat_2[tidy * stride + r];
                    }
                }
                __syncthreads();

                /* Thread level */
                z = tidy + bptr_begin;
                if(z < bptr_end) {
                    sptValue const entry = dev_values[z];
                    sptElementIndex const mode_i = dev_einds[mode][z];
                    sptElementIndex const tmp_i_1 = dev_einds[times_mat_index_1][z];
                    sptElementIndex const tmp_i_2 = dev_einds[times_mat_index_2][z];

                    sptValue * const bmvals_row = sm_blocked_mvals + mode_i * R;
                    sptValue * const blocked_times_mat_1_row = sm_blocked_times_mat_1 + tmp_i_1 * R;
                    sptValue * const blocked_times_mat_2_row = sm_blocked_times_mat_2 + tmp_i_2 * R;

                    sptValue tmp_val = 0;
                    for(sptIndex l=0; l<num_loops_r; ++l) {
                        r = tidx + l * blockDim.x;
                        tmp_val = entry * blocked_times_mat_1_row[r] * blocked_times_mat_2_row[r];
                        atomicAdd(&(bmvals_row[r]), tmp_val);
                    }

                    if(rest_loop > 0 && tidx < rest_loop) {
                        r = tidx + num_loops_r * blockDim.x;
                        tmp_val = entry * blocked_times_mat_1_row[r] * blocked_times_mat_2_row[r];
                        atomicAdd(&(bmvals_row[r]), tmp_val);
                    }

                }   // End loop entries

                /* Store back mats[nmodes] from shared memory */
                if (tidy < sb_size) {
                    for(sptIndex l=0; l<num_loops_r; ++l) {
                        r = tidx + l * blockDim.x;
                        atomicAdd( &(blocked_mvals[tidy * stride + r]),  sm_blocked_mvals[tidy * R + r] );
                    }
                    if(rest_loop > 0 && tidx < rest_loop) {
                        r = tidx + num_loops_r * blockDim.x;
                        atomicAdd( &(blocked_mvals[tidy * stride + r]),  sm_blocked_mvals[tidy * R + r] );
                    }
                }


            } else { /* Not enough matrix reuse */
                /* Thread level */
                z = tidy + bptr_begin;
                if(z < bptr_end) {
                    sptValue const entry = dev_values[z];
                    sptElementIndex const mode_i = dev_einds[mode][z];
                    sptElementIndex const tmp_i_1 = dev_einds[times_mat_index_1][z];
                    sptElementIndex const tmp_i_2 = dev_einds[times_mat_index_2][z];

                    sptValue * const bmvals_row = blocked_mvals + mode_i * stride;

                    sptValue tmp_val = 0;
                    for(sptIndex l=0; l<num_loops_r; ++l) {
                        r = tidx + l * blockDim.x;
                        tmp_val = entry * blocked_times_mat_1[tmp_i_1 * stride + r] * blocked_times_mat_2[tmp_i_2 * stride + r];
                        atomicAdd(&(bmvals_row[r]), tmp_val);
                    }

                    if(rest_loop > 0 && tidx < rest_loop) {
                        r = tidx + num_loops_r * blockDim.x;
                        tmp_val = entry * blocked_times_mat_1[tmp_i_1 * stride + r] * blocked_times_mat_2[tmp_i_2 * stride + r];
                        atomicAdd(&(bmvals_row[r]), tmp_val);
                    }

                }   // End loop entries
            }   // End if: block size

        }   // End if: block range
    }   // End loop blocks

}
