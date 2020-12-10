#!/usr/bin/python3

import sys 
import itertools

# input parameters
input_file = sys.argv[1]
cutoff = float(sys.argv[2])

if input_file.endswith('.txt'):
	input_file_name = input_file[:-4]

out_file = input_file_name + '.tns'
print('write to ' + out_file)
print('cutoff: ' + str(cutoff))

# Data structures to be filled
dims = []
bcoords = []
bdims = []
boffset = []
vals = []

# variables
nmodes = 0
nnz = 0
nb = 0

# temporary variables
boffset_start = 0
nnzb_withzeros = 0

count_lines = 0
line_begin = 0
line_end = 0
val_count = 0
val_start = 0
val_end = 0


boffset.append(boffset_start)

fi = open(input_file, 'r')
fo = open(out_file, 'w')
for line in fi:
	count_lines += 1

	line_array = line.rstrip().split(" ")
	# print(line_array)

	# Input A
	if(line_array[0] == 'tensor'):
		tmp_line_array = line_array[-1]
		tmp_dims = tmp_line_array[1:-2]
		tmp_dims_array = tmp_dims.split(",")
		for dim in tmp_dims_array:
			dims.append(int(dim))
		print('DIMS: ')
		print(dims)
		nmodes = len(dims)
		print('nmodes: '+str(nmodes))

	if(line_array[0] == 'actual'):
		nnz_withzeros = int(line_array[-1])
		print("A nnz with zeros: " + str(nnz_withzeros))
	# if(line_array[0] == 'A----offsets:'):
	# 	line_begin = count_lines + 1;
	if(line_array[0] == 'blockid:'):
		nb += 1
		# block coordinates extracted from "block_offsets"
		tmp_line_array = line_array[3]
		tmp_coord = list(map(int, tmp_line_array[1:-3].split(','))) 
		bcoords.append(tmp_coord)
		
		# block dimensions
		tmp_line_array = line_array[5]
		tmp_dims = list(map(int, tmp_line_array[1:-3].split(','))) 
		bdims.append(tmp_dims)
		nnzb_withzeros = 1
		for bdim in tmp_dims:
			nnzb_withzeros *= bdim
		assert(nnzb_withzeros == int(line_array[-1]))

		boffset_start += nnzb_withzeros
		boffset.append(boffset_start)

	if (len(line_array) == nnzb_withzeros):	# process value line
		bnnz = 0
		bvals = list(map(float, line_array))
		# print(bvals)

		# coordinates is order from right to left. Opposite to ITensor
		start_coord = bcoords[-1]	# block indices list to do the incremental
		bs_dims = bdims[-1]	# block sizes for this block
		tmp_coord_list = [0] * nmodes 	# coordinates insides a block
		val_loc = 0
		# print(start_coord)
		# print(bs_dims)
		# print(tmp_coord_list)
		# print('\n')

		#Write back non-zero values with its coordinates
		while tmp_coord_list[0] < bs_dims[0] and val_loc < nnzb_withzeros:	# ordered from right to left

			# set the correct starting coordinate
			for m in range(nmodes-1, 0, -1):	# from right to left to find
				# print('m: ' + str(m))
				if (tmp_coord_list[m] < bs_dims[m] - 1):
					tmp_coord_list[m] += 1
					# print('tmp_coord_list in if-branch')
					# print(tmp_coord_list)
					break
				elif (tmp_coord_list[m] == bs_dims[m] - 1):
					tmp_coord_list[m] = 0
					if (tmp_coord_list[m - 1] < bs_dims[m - 1] - 1):
						tmp_coord_list[m - 1] += 1
						# print('tmp_coord_list in elif-branch')
						# print(tmp_coord_list)
						break
					elif (tmp_coord_list[m - 1] > bs_dims[m - 1] - 1):
						print('Out of range in finding coordinates for block')
				else:
					print('Out of range in finding coordinates for block')

			if (bvals[val_loc] > cutoff):
				bnnz += 1
				boffset[-1] = boffset[-2] + bnnz
				# Write nonzero to the output tns file
				for tmp_m in range(0, nmodes, 1):
					fo.write(str(tmp_coord_list[tmp_m] + start_coord[tmp_m]) + ' ')
				fo.write(str(bvals[val_loc]) + '\n')

			val_loc += 1
		nnz += bnnz
		

fi.close()
fo.close()

print('nb: ' + str(nb))
product = 1.0
for dim in dims:
	product *= dim
density = float(nnz) / float(product)
print('NNZ: %d, DENSITY: %.2e' % (nnz, density))
# print('bcoords:')
# print(bcoords)
# print('bdims:')
# print(bdims)
# print('boffset:')
# print(boffset)
