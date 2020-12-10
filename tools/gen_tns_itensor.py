#!/usr/bin/python3

import sys 
import itertools

# input parameters
input_file = sys.argv[1]
cutoff = float(sys.argv[2])

if input_file.endswith('.txt'):
	input_file_name = input_file[:-4]

out_A = input_file_name + '_A.tns'
out_B = input_file_name + '_B.tns'
print('write 1st tensor to ' + out_A)
print('write 2nd tensor to ' + out_B)
print('cutoff: ' + str(cutoff))

# Data structures from input
dims_A = [129,4,184,24,4]	# block dimension sizes
dims_B = [24,36,4,4]	# block dimension sizes
bs_A = [[1,1,1,1,1,1,2,1,2,3,2,3,4,1,1,2,3,2,2,4,4,5,3,1,4,4,1,3,5,2,1,4,4,5,1,1,1,3,4,4,1,3,3,1,1,1,2,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1],[1,1,1,1],[1,1,1,4,1,2,4,6,1,2,6,4,6,6,10,1,1,2,5,2,4,6,6,6,2,9,1,1,2,1,4,6,4,2,7,2,4,4,3,4,1,1,1,1,1,3,2,4,4,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1],[4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1],[1,1,1,1]]
bs_B = [[4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1],[4,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1],[1,1,1,1],[1,1,1,1]]

# block dimension offset
bs_A_offset = []
bs_B_offset = []
for bs_dim in bs_A:
	offset = [0,]
	for bs in bs_dim[:-1]:
		offset.append(offset[-1] + bs)
	bs_A_offset.append(offset)
for bs_dim in bs_B:
	offset = [0,]
	for bs in bs_dim[:-1]:
		offset.append(offset[-1] + bs)
	bs_B_offset.append(offset)
print(bs_A_offset)
print(bs_B_offset)

# Data structures to be filled
bcoords_A = []
bcoords_B = []
boffset_A = []
boffset_B = []
vals_A = []
vals_B = []

# variables
nnz_A = 0
nnz_B = 0
nb_A = 0
nb_B = 0
count_lines = 0
line_begin = 0
line_end = 0
val_count = 0
val_start_A = 0
val_end_A = 0
val_start_B = 0
val_end_B = 0


nmodes_A = len(dims_A)
nmodes_B = len(dims_B)

# check the block sizes for A
for mode in range(0, len(bs_A), 1):
	sum_bs_mode = 0
	for bs in bs_A[mode]:
		sum_bs_mode += bs
	# print(sum_bs_mode)
	assert(sum_bs_mode == dims_A[mode])

# check the block sizes for B
for mode in range(0, len(bs_B), 1):
	sum_bs_mode = 0
	for bs in bs_B[mode]:
		sum_bs_mode += bs
	# print(sum_bs_mode)
	assert(sum_bs_mode == dims_B[mode])



fi = open(input_file, 'r')
for line in fi:
	count_lines += 1

	line_array = line.rstrip().split(" ")
	# print(line_array)

	# Input A
	if(line_array[0] == 'A----size():'):
		nnz_A = int(line_array[-1])
		print('\n### A ###')
		print('nmodes_A: '+str(nmodes_A))
		print("A nnz: " + str(nnz_A))
	if(line_array[0] == 'A----offsets:'):
		line_begin = count_lines + 1;
	if(line_array[0] == 'A-Block:'):
		tmp_line_array = line_array[1]
		tuple_coord = tuple(map(int, tmp_line_array[:-1].split(','))) 
		bcoords_A.append(tuple_coord)
		boffset_A.append(int(line_array[-1]))
		if(nnz_A > 0 and nnz_B == 0): 
			nb_A += 1

	if(line_array[0] == 'A----data:'):
		line_end = count_lines;
		# print('line_begin: ' + str(line_begin) + ', line_end: ' + str(line_end))
		print('A #nnz blocks: ' + str(nb_A))
		assert(line_end - line_begin == nb_A)
		boffset_A.append(nnz_A)	# An additional offset 
		assert(len(bcoords_A) == nb_A)
		assert(len(boffset_A) == nb_A + 1)
		print('bcoords_A:')
		print(bcoords_A)
		print('boffset_A:')
		print(boffset_A)

		val_start_A = count_lines + 1	# why +1 not +2?
		val_end_A = count_lines + nnz_A + 1
		print('val_start_A: ' + str(val_start_A) + ', val_end_A: ' + str(val_end_A))

	# Input B
	if(line_array[0] == 'B----size():'):
		nnz_B = int(line_array[-1])
		print('\n### B ###')
		print('nmodes_B: '+str(nmodes_B))
		print("B nnz: " + str(nnz_B))
	if(line_array[0] == 'B----offsets:'):
		line_begin = count_lines + 1;
	if(line_array[0] == 'B-Block:'):
		tmp_line_array = line_array[1]
		tuple_coord = tuple(map(int, tmp_line_array[:-1].split(','))) 
		bcoords_B.append(tuple_coord)
		boffset_B.append(int(line_array[-1]))
		if(nnz_A > 0 and nnz_B > 0): 
			nb_B += 1

	if(line_array[0] == 'B----data:'):
		line_end = count_lines;
		# print('line_begin: ' + str(line_begin) + ', line_end: ' + str(line_end))
		print('B #nnz blocks: ' + str(nb_B))
		assert(line_end - line_begin == nb_B)
		boffset_B.append(nnz_B)	# An additional offset 
		assert(len(bcoords_B) == nb_B)
		assert(len(boffset_B) == nb_B + 1)
		print('bcoords_B:')
		print(bcoords_B)
		print('boffset_B:')
		print(boffset_B)

		val_start_B = count_lines + 1	# why +1 not +2?
		val_end_B = count_lines + nnz_B + 1
		print('val_start_B: ' + str(val_start_B) + ', val_end_B: ' + str(val_end_B))

fi.close()

fi = open(input_file, 'r')
for line in itertools.islice(fi, val_start_A, val_end_A):
	line_array = line.rstrip().split(" ")
	vals_A.append(float(line_array[-1]))
fi.close()
assert(len(vals_A) == nnz_A)
print('\nvals_A:')
print(vals_A)

fi = open(input_file, 'r')
for line in itertools.islice(fi, val_start_B, val_end_B):
	line_array = line.rstrip().split(" ")
	vals_B.append(float(line_array[-1]))
fi.close()
assert(len(vals_B) == nnz_B)
print('vals_B:')
print(vals_B)

# Generate COO for A
fA = open(out_A, 'w')
for ind in range(0, nb_A, 1):	# loop for all blocks
	offset_start = boffset_A[ind]
	offset_end = boffset_A[ind + 1]
	bnnz = offset_end - offset_start	# nnz in this block
	start_coord_A = bcoords_A[ind]	# block indices to do the incremental
	start_coord_A_list = list(start_coord_A)	# temporary for modifying a tuple
	tmp_coord_A_list = [0] * nmodes_A 	# coordinates insides a block
	bs_dims_A = [0] * nmodes_A	# block sizes for this block
	for m in range(0, nmodes_A, 1):
		bs_dims_A[m] = bs_A[m][start_coord_A_list[m]]
	# print(bs_dims_A)
	bnnz_loc = 0

	while tmp_coord_A_list[nmodes_A-1] < bs_dims_A[nmodes_A-1] and bnnz_loc < bnnz:
		# set the correct starting coordinate
		for m in range(0, nmodes_A-1, 1):	# from left to right to find
			if (tmp_coord_A_list[m] == bs_dims_A[m] - 1):
				tmp_coord_A_list[m] = 0
				if (tmp_coord_A_list[m + 1] < bs_dims_A[m + 1] - 1):
					tmp_coord_A_list[m + 1] += 1
					break
		# print(tmp_coord_A_list)

		# write the first nnz
		if (vals_A[offset_start + bnnz_loc] > cutoff):
			for tmp_m in range(0, nmodes_A, 1):
				fA.write(str(tmp_coord_A_list[tmp_m] + bs_A_offset[tmp_m][start_coord_A_list[tmp_m]]) + ' ')
			fA.write(str(vals_A[offset_start + bnnz_loc]) + '\n')
		bnnz_loc += 1

		for z in range(1, bs_dims_A[0], 1):
			tmp_coord_A_list[0] += 1	# Only increment mode-0 by 1
			if(bnnz_loc < bnnz):
				if(vals_A[offset_start + bnnz_loc] > cutoff):
					for tmp_m in range(0, nmodes_A, 1):
						fA.write(str(tmp_coord_A_list[tmp_m] + bs_A_offset[tmp_m][start_coord_A_list[tmp_m]]) + ' ')
					fA.write(str(vals_A[offset_start + bnnz_loc]) + '\n')
			bnnz_loc += 1

	# print('\n')
	# fA.write('\n')

fA.close()

# Generate COO for B
fB = open(out_B, 'w')
for ind in range(0, nb_B, 1):	# loop for all blocks
	offset_start = boffset_B[ind]
	offset_end = boffset_B[ind + 1]
	bnnz = offset_end - offset_start	# nnz in this block
	start_coord_B = bcoords_B[ind]	# block indices to do the incremental
	start_coord_B_list = list(start_coord_B)	# temporary for modifying a tuple
	tmp_coord_B_list = [0] * nmodes_B 	# coordinates insides a block
	bs_dims_B = [0] * nmodes_B	# block sizes for this block
	for m in range(0, nmodes_B, 1):
		bs_dims_B[m] = bs_B[m][start_coord_B_list[m]]
	# print(bs_dims_B)
	bnnz_loc = 0

	while tmp_coord_B_list[nmodes_B-1] < bs_dims_B[nmodes_B-1] and bnnz_loc < bnnz:
		# set the correct starting coordinate
		for m in range(0, nmodes_B-1, 1):	# from left to right to find
			if (tmp_coord_B_list[m] == bs_dims_B[m] - 1):
				tmp_coord_B_list[m] = 0
				if (tmp_coord_B_list[m + 1] < bs_dims_B[m + 1] - 1):
					tmp_coord_B_list[m + 1] += 1
					break
		# print(tmp_coord_B_list)

		# write the first nnz
		if(vals_B[offset_start + bnnz_loc] > cutoff):
			for tmp_m in range(0, nmodes_B, 1):
				fB.write(str(tmp_coord_B_list[tmp_m] + bs_B_offset[tmp_m][start_coord_B_list[tmp_m]]) + ' ')
			fB.write(str(vals_B[offset_start + bnnz_loc]) + '\n')
		bnnz_loc += 1

		for z in range(1, bs_dims_B[0], 1):
			tmp_coord_B_list[0] += 1	# Only increment mode-0 by 1
			if(bnnz_loc < bnnz):
				if(vals_B[offset_start + bnnz_loc] > cutoff):
					for tmp_m in range(0, nmodes_B, 1):
						fB.write(str(tmp_coord_B_list[tmp_m] + bs_B_offset[tmp_m][start_coord_B_list[tmp_m]]) + ' ')
					fB.write(str(vals_B[offset_start + bnnz_loc]) + '\n')
			bnnz_loc += 1

	# print('\n')
	# fB.write('\n')

fB.close()