#!/usr/bin/python
# this is the tMRCA changed or not detection algorithm implementation; I want to first of all try this algorithm.
# logs:
# version: 1.0; this is a testing version, aiming to get the tMRCA unchanged pairs and chagned pairs, and then verifying them with the previous stack-based method
# I will test unchanged pairs between short intervals and large intervals, and changed pairs between short intervals and large intervals.
# algorithm: (based on previous caldurpar.py algorithm)
# ###please refer to the document of paper version


# the following stack-based parsing algorithm is a reference
# =================================== the previous parsing algorithm ==============================
# 1. initialize two list, one used to put the node, and another used to put the MRCA of the corresponding node (linked by the index of the two list)
# 2. we should use a pointer to pointing the present working sub-list (in both list1 and list2); this pointer will be used later on to detect the only-left sub-list in the parent-list -> WHY?
# 3. when we encounter a "(", create a new sub-list in list1, and create the corresponding possition in list2 (using None for further detection)
# 4. when encounter a node, add it to the present working list, and add its MRCA ("," will be ignored) to list2
# 5. when encounter a ")", move the nodes in present sub-list to the previous one, and drop the following MRCA of that sub-list; but, if this is the first sub-list, we should not do this, we should add the following MRCA to the present one; if this is a left leaf, we should move the nodes to the previous sub-list, and add the two MRCA (using None to detect the left leaf; otherwise there will be an existing MRCA as its left sub-tree has been traversalled)
# =================================================================================================


# Jun 17,
# description:   I want to write a comparison script of the new algorithm and the naive one
# function:      Given a tree file, we go through all the trees, and detect the unchanged pairs using both the new algorithm and the naive one, then report the time; we should compare their consistency and the same time.
# modification:  We should use the range tricks to perform the intersection operation, for a more real speedup ratio.


# the overall structure:
# for i = 1 : # of trees:
# 	processing (tree[i], algorithm#1, timer#1)
# 	processing (tree[i], algorithm#2, timer#2)
# 	if in-consistency for the two results:
# 		break (if this really happens, it should be terrible)
# 	report the speedup of:
# 		-> this time
# 		-> overall
# end


import os
import sys
import time
import re
import math
from algorithm_new import *
from algorithm_old import *


# testing tree samples:
# these trees are used only for debugging
#tree1 = "	tree NumGen_tree_1_26_pos_31973 = [&U] ((((7.1:332.916, 6.1:332.916):3351.067, 1.1:3683.983):1856.519, 8.1:5540.502):12591.547, ((5.1:691.329, 9.1:691.329):10786.342, (2.1:6848.286, ((4.1:837.400, 10.1:837.400):2956.146, 3.1:3793.546):3054.740):4629.385):6654.378);"
#tree2 = "	tree NumGen_tree_1_27_pos_34154 = [&U] (((((4.1:837.400, 10.1:837.400):2052.195, (7.1:332.916, 6.1:332.916):2556.680):794.387, 1.1:3683.983):1856.519, 8.1:5540.502):12591.547, ((5.1:691.329, 9.1:691.329):10786.342, (2.1:6848.286, 3.1:6848.286):4629.385):6654.378);"
tree1 = "	tree NumGen_tree_1_12091_pos_21948842 = [&U] ((3.1:992.923, (1.1:539.208, 9.1:539.208):453.715):8093.405, ((10.1:237.542, 5.1:237.542):5964.348, ((4.1:5071.138, 2.1:5071.138):1130.377, (7.1:535.796, (8.1:70.626, 6.1:70.626):465.170):5665.719):0.375):2884.437);"
tree2 = "	tree NumGen_tree_1_12092_pos_21949317 = [&U] ((3.1:992.923, (1.1:539.208, 9.1:539.208):453.715):8093.405, ((2.1:2996.869, (10.1:237.542, 5.1:237.542):2759.327):3205.021, (4.1:6201.515, (7.1:535.796, (8.1:70.626, 6.1:70.626):465.170):5665.719):0.375):2884.437);"


#TREE_WHOLE=re.compile(r'^tree\s(.*)')
#TREE_NUMBER_BREAKPOINT=re.compile(r'_(\d+)_(\d+)_pos_(\d+)\s')
#TREE_CONTENT=re.compile(r'\((.*)\);')
NODE=re.compile(r':')
DOT=re.compile(r'\.')
#END="100000000"  # we should manually set this
SAMPLE = 2000
TOLERANCE = 0.01
SPEEDUP = None


def algorithm1(tree1, tree2):
	result = []
	time1 = time.time()

	# here are some important points!!!
	# the h1 and h2 are the operating hash table, which should simulate the behavior of the stack
	# the h11 and h21 are the results hash table, which contain all the final results;
	# we get the final results for all the internal nodes only after they have both the two sub-trees
	h1 = {}
	h11 = {}
	list1 = []
	list2 = [0] * SAMPLE  # then we can add one element to one specific position
	parser1(tree1, h1, h11, list1, list2)
	h2 = {}
	h21 = {}
	list3 = []
	#hash_final = {}  # instead we use the hash table for a quick searching
	# instead we use a list of lists to store all the intersection lists; we report the pairs when comparing
	parser2(tree2, h11, h2, h21, list1, list2, list3, result)  # we now report the changed/unchanged pairs directly in parser2
	
	time2 = time.time()
	timer = time2 - time1

	return (result, timer)


def algorithm2(tree1, tree2):
	result = {}
	time1 = time.time()

	h3 = {}  # store the all-pair tMRCA in this hashing
	cal_during_par(tree1, h3)
	h4 = {}  # the same as h3
	cal_during_par(tree2, h4)

	for pair in h3:
		if h3[pair] - h4[pair] <= TOLERANCE and h3[pair] - h4[pair] >= -TOLERANCE:
			result[pair] = 1

	time2 = time.time()
	timer = time2 - time1

	return (result, timer)


def comparison(result1, result2):
	for l in result1:  # result1: [[[x, x, x], [x, x, x]], [[x, x, x], [x, x, x]], [], [], [] ...]
		# each of l is a intersection, in which there are two sub-lists
		left, right = l
		for leaf1 in left:
			for leaf2 in right:
				if leaf1 < leaf2:
					pair = str(leaf1) + "-" + str(leaf2)
				else:
					pair = str(leaf2) + "-" + str(leaf1)
				# DEBUG
				#print "the new result:",
				#print pair
				if pair not in result2:
					# DEBUG
					#print pair
					return 0
				result2[pair] = 0
	num = 0
	for pair in result2:
		if result2[pair] != 0:
			# DEBUG
			#print "the left old result:",
			#print pair
			num+=1
	if num != 0:
		return 0

	return 1


def report(seq, timer1, timer2):
	global SPEEDUP
	speedup = timer2/timer1
	if seq == 1:
		SPEEDUP = speedup
	else:
		SPEEDUP = speedup/seq + SPEEDUP*(seq-1)/seq
	# is there any problem about the speedup? ....
	
	print "iteration:",
	print seq,
	print "one-time speedup:",
	print speedup,
	print "overall speedup:",
	print SPEEDUP

	return


if __name__ == "__main__":
	# the global SPEEDUP
	SPEEDUP = None
	if len(sys.argv) != 2:
		print "Please follow this format to run the program:"
		print "\"python test.py filename.trees\" or \"./test.py filename.trees\"."
		sys.exit()
	fname = sys.argv[1]
	
	predir = os.getcwd()
	file = open(predir + "/" + fname, "r")
	lines = file.readlines()
	file.close()
	"""
	for n in range(0, len(lines)-5000):
		tree1 = lines[n].strip()
		tree2 = lines[n+5000].strip()
		
		# the new algorithm:
		# the result1 is a list of lists storing all the intersections
		(result1, timer1) = algorithm1(tree1, tree2)

		# the old algorithm:
		# the result2 is a hashing table
		(result2, timer2) = algorithm2(tree1, tree2)

		success = comparison(result1, result2)

		if not success:
			print "Oh there are some errors in the new algorithm!!"
			print "program terminate..."
			break

		report(n+1, timer1, timer2)
	"""
	tree1 = lines[0].strip()
	tree2 = lines[1].strip()
	
	# the new algorithm:
	# the result1 is a list of lists storing all the intersections
	(result1, timer1) = algorithm1(tree1, tree2)

	# the old algorithm:
	# the result2 is a hashing table
	(result2, timer2) = algorithm2(tree1, tree2)

	success = comparison(result1, result2)

	if not success:
		print "Oh there are some errors in the new algorithm!!"
		print "program terminate..."
	else:
		print "hey there are no errors thanks!.."

	report(1, timer1, timer2)

