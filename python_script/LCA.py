#!/usr/bin/python
# function: the LCA algorithm implementation; linear time pre-processing and constant time query; in practice, give two leaves, and send back their tMRCA value; this algorithm should be based on the previous stack-based parsing algorithm
# version: 1.0; Jun.18; finishing at Jun.19


# we provide two trees here for testing
tree = "	tree NumGen_tree_1_12091_pos_21948842 = [&U] ((3.1:992.923, (1.1:539.208, 9.1:539.208):453.715):8093.405, ((10.1:237.542, 5.1:237.542):5964.348, ((4.1:5071.138, 2.1:5071.138):1130.377, (7.1:535.796, (8.1:70.626, 6.1:70.626):465.170):5665.719):0.375):2884.437);"
#tree = "	tree NumGen_tree_1_12092_pos_21949317 = [&U] ((3.1:992.923, (1.1:539.208, 9.1:539.208):453.715):8093.405, ((2.1:2996.869, (10.1:237.542, 5.1:237.542):2759.327):3205.021, (4.1:6201.515, (7.1:535.796, (8.1:70.626, 6.1:70.626):465.170):5665.719):0.375):2884.437);"


import re
import os
import math
import time


#TREE_WHOLE=re.compile(r'^tree\s(.*)')
#TREE_NUMBER_BREAKPOINT=re.compile(r'_(\d+)_(\d+)_pos_(\d+)\s')
#TREE_CONTENT=re.compile(r'\((.*)\);')
NODE=re.compile(r':')
DOT=re.compile(r'\.')
SAMPLE = 10
BLOCK_SIZE = None
BLOCK_NUM = None
BLOCK_SIZE_LAST = None
MODE_BLOCK = {}
MODE_BLOCK_LAST = {}
TOLERANCE = 0.01


def cal_during_par(tree, htable):  # the prvious stack-based parsing function
	n = len(tree)
	i = 0
	pointer = -1
	l1 = []  # store the nodes and their existing list
	l2 = []  # store the MRCA of corresponding nodes
	# special case for "2.1:11018.678, (1.1:5151.520, (3.1:3913.383, 4.1:3913.383):1238.136):5867.158"
	while tree[i] != "(":
		i+=1

	while i<n:
		if tree[i] == "," or tree[i] == " " or tree[i] == ";":
			i+=1
			continue

		if tree[i] == "(":
			i+=1
			pointer += 1
			l1.append([])
			l2.append(None)
			continue

		if tree[i].isdigit():
			s=""
			while tree[i]!="," and tree[i]!=")":
				s=s+tree[i]
				i=i+1
			data=NODE.split(s)
			node=DOT.split(data[0])[0]
			MRCA=float(data[1])
			l1[pointer].append(node)
			if l2[pointer] == None:  # two sub-nodes
				l2[pointer]=MRCA
			#------- here we should report!! -------
			for j in range(pointer+1):
				for node1 in l1[j]:
					if node != node1:  # efficiency?
						if int(node) < int(node1):
							name = node + "-" + node1
						else:
							name = node1 + "-" + node
						htable[name] = l2[j]
			continue

		if tree[i] == ")":  # there may be a ":" following the ")"
			if tree[i+1] != ";":
				if pointer == 0:  # on the leftmost branch
					#l1[pointer] remains the same
					i+=2
					s=""
					while tree[i]!="," and tree[i]!=")":
						s=s+tree[i]
						i=i+1
					l2[pointer]+=float(s)
				else:  # two possibilities: left leaf and right leaf
					l1[pointer-1].extend(l1[pointer])
					l1.pop()
					i+=2
					s=""
					while tree[i]!="," and tree[i]!=")":
						s=s+tree[i]
						i=i+1
					if l2[pointer-1] == None:  # left leaf
						l2[pointer-1]=l2[pointer]+float(s)
						l2.pop()
					else:  # right leaf, the MRCA has existed	
						l2.pop()
					pointer-=1
			else:
				i+=1
			continue
	return

#================================================= the Euler tour ========================================
def Euler(tree, E, L, r, R):  # a simple and subtle parser
	# type: linear time Euler tour to get all the necessary lists
	# the description for the E L r and R:
	# E: not initialized; put all the labels
	# L: not initialized; put the levels of all nodes; so using the stack to help
	# r: not initialized; put the tMRCA value of the internal nodes
	# R: initialized; use the names of the nodes to index the positions of these nodes' positions in E
	n = len(tree)
	i = 0

	# the following stack and l2 are all local things, which will later be dropped.
	stack = []  # used to store all the internal nodes temporarily
	parent = 0  # record the in-time present internal node in the stack

	l2 = []  # store the MRCA of corresponding nodes; initialized as None

	while tree[i] != "(":
		i+=1

	while i<n:
		if tree[i] == " " or tree[i] == ";":
			i+=1
			# do nothing here!!
			continue

		if tree[i] == ",":
			i+=1
			E.append(stack[-1])
			L.append(len(stack)-1)
			continue

		if tree[i] == "(":
			i+=1
			stack.append(parent)
			E.append(stack[-1])
			L.append(len(stack)-1)
			r.append(None)
			parent+=1
			l2.append(None)
			continue

		if tree[i].isdigit():
			s=""
			while tree[i]!="," and tree[i]!=")":
				s=s+tree[i]
				i=i+1
			data=NODE.split(s)
			node=int(DOT.split(data[0])[0])
			MRCA=float(data[1])
			E.append(node)
			L.append(len(stack))
			R[node-1] = len(E)-1

			if l2[-1] == None:  # this is the left sub-tree
				l2[-1]=MRCA

		if tree[i] == ")":  # there may be a ":" following the ")"
			if tree[i+1] != ";":
				# two possibilities: left leaf and right leaf
				i+=2
				s=""
				while tree[i]!="," and tree[i]!=")":
					s=s+tree[i]
					i=i+1
				tP=float(s)

				E.append(stack[-1])
				L.append(len(stack)-1)
				r[E[-1]]=l2[-1]
				stack.pop()
				if l2[-2] == None:  # back from the left sub-tree
					MRCA = l2[-1] + tP
					l2[-2]=MRCA
				# l2 is a in-time stack
				l2.pop()
			else:
				i+=1
				E.append(stack[-1])
				L.append(len(stack)-1)
				r[E[-1]]=l2[-1]
				stack.pop()
				l2.pop()
			continue
	return
#====================================================================================================


#======================================= RMQ pre-processing =========================================
def find_mode(list_temp, MODE_BLOCK, BLOCK_SIZE):
	# type: linear time pre-processing
	# input: block list, MODE_BLOCK
	# output: block type, minimum position, minimum value
	key = ""
	for i in range(1, len(list_temp)):
		if list_temp[i] > list_temp[i-1]:
			key+="1"
		else:
			key+="0"
	query = "0-" + str(BLOCK_SIZE-1)
	pos = MODE_BLOCK[key][query]
	val = list_temp[pos]
	return (val, pos, key)

def Dynamic(list_up_val):
	# type: dynamic programming, nlogn time where n is the length of the list
	# input: a list containing numbers waiting to be compared
	# output: two hashing tables, of which one record the minimum value of all the queries, and
	#	the other the in-list position of that minimum value; the query are length of 2^k
	hashtable_val = {}
	hashtable_pos = {}
	
	N = len(list_up_val)
	expo = 1
	while 1:
		for i in range(0, N):
			if i + (expo-1) >= N:
				break
			key = str(i) + "-" + str(i+expo-1)
			if expo == 1:
				hashtable_val[key] = list_up_val[i]
				hashtable_pos[key] = i
			else:
				key1 = str(i) + "-" + str(i+(expo/2)-1)
				key2 = str(i+(expo/2)) + "-" + str(i+expo-1)
				if hashtable_val[key1] < hashtable_val[key2]:
					hashtable_val[key] = hashtable_val[key1]
					hashtable_pos[key] = hashtable_pos[key1]
				else:
					hashtable_val[key] = hashtable_val[key2]
					hashtable_pos[key] = hashtable_pos[key2]
		expo=expo*2
		if expo > N:
			break
	return (hashtable_val, hashtable_pos)

def RMQ(L):
	# type: total linear time pre-processing; two parts: linear time mode finding, and dynamic ST filling
	# input: the list of node levels
	# output: list_up_val, list_up_pos, list_up_type, upResult_val, upResult_pos
	global BLOCK_SIZE
	global BLOCK_NUM
	global BLOCK_SIZE_LAST
	global MODE_BLOCK
	global MODE_BLOCK_LAST

	#================== fill the upper list, with value, position and block type ==================
	list_up_val = [0] * BLOCK_NUM
	list_up_pos = [0] * BLOCK_NUM
	list_up_type = [0] * BLOCK_NUM

	for i in range(BLOCK_NUM):
		list_temp = L[i*BLOCK_SIZE: (i+1)*BLOCK_SIZE]
		if i != BLOCK_NUM-1:
			(val, pos, ty) = find_mode(list_temp, MODE_BLOCK, BLOCK_SIZE)
			list_up_val[i] = val
			list_up_pos[i] = pos
			list_up_type[i] = ty
		else:
			(val, pos, ty) = find_mode(list_temp, MODE_BLOCK_LAST, BLOCK_SIZE_LAST)
			list_up_val[i] = val
			list_up_pos[i] = pos
			list_up_type[i] = ty
	
	# DEBUG
	#print list_up_val
	#print list_up_pos
	#print list_up_type
	#=============================================================================================
	
	#===================================== dynamic ST filling ====================================
	# now we have these two lists; we should further perform the nlogn processing	
	(upResult_val, upResult_pos) = Dynamic(list_up_val)
	
	# DEBUG
	#print upResult_val
	#print upResult_pos
	#============================================================================================

	# the following lists or tables are important to get the final results: (transfer back if needed!!)
	# list_up_pos, list_up_type, upResult_val, upResult_pos, (global: L, MODE_BLOCK, MODE_BLOCK_LAST)

	return (list_up_pos, list_up_type, upResult_val, upResult_pos)
#======================================================================================================


#======================================== constant tMRCA query ========================================
def getMRCA(leaf1, leaf2, R, L, E, r, list_up_pos, list_up_type, upResult_val, upResult_pos):
	# algorithm:
	# 1. use the L to perform the RMQ algorithm;
	# 2. use the R to find the index (i,j) to be processed with RMQ;
	# 3. use the index of the RMQ and E to get the label of that internal node;
	# 4. use the r to get the actual tMRCA value

	global BLOCK_SIZE
	global BLOCK_NUM
	global BLOCK_SIZE_LAST
	global MODE_BLOCK
	global MODE_BLOCK_LAST
	
	pos1 = R[leaf1-1]
	pos2 = R[leaf2-1]
	if pos1 > pos2:  # let pos1 < pos2
		temp = pos1
		pos1 = pos2
		pos2 = temp

	# now query (pos1, pos2)
	coordinate1 = pos1*1.0/BLOCK_SIZE
	coordinate2 = pos2*1.0/BLOCK_SIZE
	if coordinate1//1 == coordinate2//1:
		# they are in the same block, so we directly find that block and then report
		block_num = int(coordinate1//1)
		pos11 = pos1 - block_num * BLOCK_SIZE
		pos22 = pos2 - block_num * BLOCK_SIZE
		query = str(pos11)+"-"+str(pos22)
		index = MODE_BLOCK[list_up_type[block_num]][query] + block_num * BLOCK_SIZE
		tMRCA = r[E[index]]
	else:
		# they are in different blocks, so we should deal them separately and then
		# deal with the upper list
		if coordinate1//1 + 1 == coordinate2//1:
			# they are in adjacent blocks
			block_num1 = int(coordinate1//1)
			pos11 = pos1 - block_num1 * BLOCK_SIZE
			query1 = str(pos11)+"-"+str(BLOCK_SIZE-1)
			index1 = MODE_BLOCK[list_up_type[block_num1]][query1] + block_num1 * BLOCK_SIZE

			block_num2 = int(coordinate2//1)
			pos22 = pos2 - block_num2 * BLOCK_SIZE
			query2 = "0-"+str(pos22)
			index2 = MODE_BLOCK[list_up_type[block_num2]][query2] + block_num2 * BLOCK_SIZE

			if L[index1] < L[index2]:
				tMRCA = r[E[index1]]
			else:
				tMRCA = r[E[index2]]
		else:
			# they are not in adjacent blocks, so there is another index to be compared
			block_num1 = int(coordinate1//1)
			pos11 = pos1 - block_num1 * BLOCK_SIZE
			query1 = str(pos11)+"-"+str(BLOCK_SIZE-1)
			index1 = MODE_BLOCK[list_up_type[block_num1]][query1] + block_num1 * BLOCK_SIZE

			block_num2 = int(coordinate2//1)
			pos22 = pos2 - block_num2 * BLOCK_SIZE
			query2 = "0-"+str(pos22)
			index2 = MODE_BLOCK[list_up_type[block_num2]][query2] + block_num2 * BLOCK_SIZE

			# process the third index on which there may be a minimum value
			# this is the range:
			# [block_num1+1, block_num2-1]
			if block_num1+1 == block_num2-1:
				# there is only one block between
				index3 = int(list_up_pos[block_num1+1] + (block_num1+1) * BLOCK_SIZE)
			else:
				# there are more than one lists between
				k = (math.log((block_num2-1) - (block_num1+1))/math.log(2))//1
				interval = int(math.pow(2, k))
				query1 = str(block_num1+1) + "-" + str(block_num1+1+interval-1)
				query2 = str(block_num2-1-interval+1) + "-" + str(block_num2-1)
				if upResult_val[query1] < upResult_val[query2]:
					index3 = upResult_pos[query1]  # this is the block number, not the actual index
				else:
					index3 = upResult_pos[query2]
				index3 = int(list_up_pos[index3] + index3 * BLOCK_SIZE)

			# now compare index1, index2 and index3 in the L
			if L[index1] < L[index2]:
				if L[index1] < L[index3]:
					tMRCA = r[E[index1]]
				else:
					tMRCA = r[E[index3]]
			else:
				if L[index2] < L[index3]:
					tMRCA = r[E[index2]]
				else:
					tMRCA = r[E[index3]]

	return tMRCA
#=====================================================================================================


#==================================== the RMQ mode filling function ==================================
#=========== this function should only be called once after we know the number of SAMPLE =============
def get_min(i, j, key):
	# type: n^2 submode filling
	# input: (i, j) i<>j, the differential lists: "010101110..."
	# output: the index in which there is a minimum value among list[>=i, <=j]
	# note: the index is the position in the previous block
	index = i
	initial = 0
	actual = initial
	for n in range(i+1, j+1):
		if key[n-1] == "0":
			actual -= 1
		else:
			actual += 1
		if actual < initial:
			initial = actual
			index = n
	return index

def mode_fill(key, N, MODE_BLOCK):
	# type: recursive mode_fill function
	# input: the number of bits in this mode
	# output: the mode directory for the mode with this length
	if N != 2:  # N = 2 is the last possible bit
		key+="0"
		mode_fill(key, N-1, MODE_BLOCK)
		key=(key[:-1])+"1"
		mode_fill(key, N-1, MODE_BLOCK)
	else:
		key+="0"
		
		# fill this mode
		MODE_BLOCK[key] = {}
		total = len(key) + 1  # this is the BLOCK_SIZE
		for i in range(total):
			for j in range(i, total):
				query = str(i) + "-" + str(j)
				MODE_BLOCK[key][query] = get_min(i, j, key)

		key=(key[:-1])+"1"

		# fill this mode
		MODE_BLOCK[key] = {}
		total = len(key) + 1  # this is the BLOCK_SIZE
		for i in range(total):
			for j in range(i, total):
				query = str(i) + "-" + str(j)
				MODE_BLOCK[key][query] = get_min(i, j, key)
	return
#====================================================================================================


if __name__ == "__main__":
	print "Hey, this is the entrance of the program! Let's get started!"

	# get the tree with 5000 chromosomes in from the tree file first
	fname = "tree_5000.trees"
	SAMPLE = 5000
	predir = os.getcwd()
	file = open(predir + "/" + fname, "r")
	lines = file.readlines()
	file.close()

	#========================================= RMQ mode filling =========================================
	N = 4*SAMPLE - 3
	# the INDIRECTION: # these data should be used by all the trees because the N will not change overall
	BLOCK_SIZE = int(((math.log(N)/math.log(2))/2)//1 + 1)
	# TODO check whether or not there is no last block??!!
	if (N*1.0/BLOCK_SIZE%1) == 0.0:
		BLOCK_NUM = int(N*1.0/BLOCK_SIZE)
		BLOCK_SIZE_LAST = BLOCK_SIZE
	else:
		BLOCK_NUM = int((N*1.0/BLOCK_SIZE)//1 + 1)
		BLOCK_SIZE_LAST = N - BLOCK_SIZE*(BLOCK_NUM-1)

	#print BLOCK_SIZE
	#print BLOCK_NUM
	#print BLOCK_SIZE_LAST

	MODE_BLOCK = {}
	MODE_BLOCK_LAST = {}

	key = ""
	# TODO when the BLOCK_SIZE is too small, it may not work
	mode_fill(key, BLOCK_SIZE, MODE_BLOCK)
	#print "this is MODE_BLOCK"
	#print MODE_BLOCK
	#print len(MODE_BLOCK)

	key = ""
	mode_fill(key, BLOCK_SIZE_LAST, MODE_BLOCK_LAST)
	#print "this is MODE_BLOCK_LAST"
	#print MODE_BLOCK_LAST
	#===================================================================================================

	for num in range(len(lines)):
		if num < 32:
			continue
		tree = lines[num].strip()
		print "this is tree#",
		print num

		time1 = time.time()

		#=========== Euler tour for a tree to get the E[1,...,2n-1], L[1,...,2n-1], R[1,...,n] =============
		E = []  # used as the list of labels
		L = []  # used as the list of levels
		r = []  # used for the internal nodes
		R = [0] * SAMPLE  # used for the leaves; use the names of nodes to index
		Euler(tree, E, L, r, R)
		
		#print "this is E:"
		#print E
		#print "this is L:"
		#print L
		#print "this is r:"
		#print r
		#print "this is R:"
		#print R
		#===================================================================================================

		#================================= RMQ pre-processing from here ====================================
		# TODO should we make the returned things local in this env?
		(list_up_pos, list_up_type, upResult_val, upResult_pos) = RMQ(L)
		#===================================================================================================	


		time2 = time.time()


		# the previous n^2 algorithm
		htable = {}
		cal_during_par(tree, htable)


		time3 = time.time()


		# from here start to compare the two algorithms
		print "new running time:",
		print time2-time1
		print "old running time:",
		print time3-time2
		print "begin comparing..."

		for i in range(1, SAMPLE+1):
			if i%500 == 0:
				print i
			for j in range(i+1, SAMPLE+1):
				leaf1 = i
				leaf2 = j
				tMRCA = getMRCA(leaf1, leaf2, R, L, E, r, list_up_pos, list_up_type, upResult_val, upResult_pos)
				pair = str(i) + "-" + str(j)
				if tMRCA != htable[pair]:
					# if there are errors, we report; but by no means will we break
					print "Bing!!!",
					print i,
					print j,
					print tMRCA,
					print htable[pair]
