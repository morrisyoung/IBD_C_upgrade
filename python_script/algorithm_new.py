#!/usr/bin/python
# function: the new algorithm implementation

import re

NODE=re.compile(r':')
DOT=re.compile(r'\.')
SAMPLE = 10
TOLERANCE = 0.01


def parser1(tree, h1, h11, list1, list2):  # deal with the first tree
	# the stack is local; should release after we finish using it
	stack = []

	# to get the actual start of a tree; from the "(" to start
	n = len(tree)
	i = 0
	while tree[i] != "(":
		i+=1

	while i<n:
		if tree[i] == "," or tree[i] == " " or tree[i] == ";":
			i+=1
			continue

		if tree[i] == "(":
			i+=1
			stack.append(None)
			continue

		if tree[i].isdigit():
			s=""
			while tree[i]!="," and tree[i]!=")":
				s=s+tree[i]
				i=i+1
			# sample data: 4.1:5071.138
			data=NODE.split(s)
			# name of node and the tMRCA are all numbers
			node=int(DOT.split(data[0])[0])
			MRCA=float(data[1])
			
			# for the two lists:
			# the first one record the actual values of all the leaves;
			# the second one use (node-1) to index their actual positions in the first list
			list1.append(node)
			list2[node-1] = list1.index(node)
			
			if stack[-1] == None:
				stack[-1] = MRCA
				if (MRCA//1) in h1:
					h1[MRCA//1].append([MRCA, list2[node-1], list2[node-1], None])
				else:
					h1[MRCA//1] = [[MRCA, list2[node-1], list2[node-1], None]]
			else:
				# we have used h1 as a stack, so just use the last element
				h1[stack[-1]//1][-1][3] = list2[node-1]
			continue

		if tree[i] == ")":
			if tree[i+1] != ";":
				# here we can get a tP value
				i+=2
				s=""
				while tree[i]!="," and tree[i]!=")":
					s=s+tree[i]
					i=i+1
				tP=float(s)
				
				#list_temp = h1[stack[-1]//1][-1][1] + h1[stack[-1]//1][-1][2]
				list_temp = [h1[stack[-1]//1][-1][1], h1[stack[-1]//1][-1][3]]

				# add to the final result hashing table
				list_temp1 = h1[stack[-1]//1].pop()
				if stack[-1]//1 in h11:
					h11[stack[-1]//1].append(list_temp1)
				else:
					h11[stack[-1]//1] = [list_temp1]

				MRCA = stack.pop() + tP
				
				# judge this is a left sub-tree or a right sub-tree
				if stack[-1] == None:
					# this is a left sub-tree
					stack[-1] = MRCA
					if (MRCA//1) in h1:
						h1[MRCA//1].append([MRCA, list_temp[0], list_temp[1], None])
					else:
						h1[MRCA//1] = [[MRCA, list_temp[0], list_temp[1], None]]
				else:
					# this is a right sub-tree
					h1[stack[-1]//1][-1][3] = list_temp[1]
			else:
				# here we are finished the parsing; just continue
				i+=1

				# remember to move up here!!!
				list_temp1 = h1[stack[-1]//1].pop()
				if stack[-1]//1 in h11:
					h11[stack[-1]//1].append(list_temp1)
				else:
					h11[stack[-1]//1] = [list_temp1]	
			continue
	return


def intersection(MRCA, list_left, list_right, h1, list1, result):  # report everything here temporarily
	# the list_left and list_right are all candidate lists
	#list_temp = h1[MRCA//1]  # should be like: [ [tMRCA1, start, middle, end], [tMRCA2, start, middle, end], [tMRCA3, start, middle, end], [tMRCA4, start, middle, end] ...          ]

	for i in range(3):
		# LATER WE CAN TRY THE XX.XX HASHING METHOD TO SEE THE SPEEDUP
		if i == 0:
			if ((MRCA//1 + 1) not in h1) or (((MRCA//1 + 1) - MRCA) > TOLERANCE):
				continue
			list_temp = h1[MRCA//1+1]
		elif i == 1:
			if (MRCA//1) not in h1:
				continue
			list_temp = h1[MRCA//1]
		else:
			if ((MRCA//1 - 1) not in h1) or ((MRCA - MRCA//1) > TOLERANCE):
				continue
			list_temp = h1[MRCA//1-1]

		list_can = []
		for element in list_temp:
			if element[0] - MRCA <= TOLERANCE and element[0] - MRCA >= -TOLERANCE:
				# this is a candidate
				list_can.append(element)
		for can in list_can:
			# can: [MRCA, start, middle, end], the previous positions in the list
			# list_left: [...]/ list_right: [...], also the previous positions in the list1

			#perform the intersection:
			list_left_can = []
			list_right_can = []
			for leaf in list_left:
				#if leaf in can[1]:  # here it is !!
				if leaf >= can[1] and leaf <= can[2]:
					list_left_can.append(list1[leaf])

			for leaf in list_right:
				#if leaf in can[2]:
				if leaf >= can[2]+1 and leaf <= can[3]:
					list_right_can.append(list1[leaf])
			"""
			if len(list_left_can) != 0 and len(list_right_can) != 0:
				# we can't remove them!!!
				# even if we have intersection, we still can't move them; because in the future we may use them again to construct the intersection; so, how to detect the changed pairs?
				#report here
				#we have list_left_can and list_right_can here, with both previous positions in
				for leaf1 in list_left_can:
					for leaf2 in list_right_can:
						if int(list1[leaf1]) < int(list1[leaf2]):
							name = str(list1[leaf1]) + "-" + str(list1[leaf2])
						else:
							name = str(list1[leaf2]) + "-" + str(list1[leaf1])
						hash_final[name] = [can[0], MRCA]
			"""
			if len(list_left_can) != 0 and len(list_right_can) != 0:
				result.append([list_left_can, list_right_can])


			#perform the reversed intersection:
			list_left_can = []
			list_right_can = []
			for leaf in list_right:
				#if leaf in can[1]:
				if leaf >= can[1] and leaf <= can[2]:
					list_left_can.append(list1[leaf])

			for leaf in list_left:
				#if leaf in can[2]:
				if leaf >= can[2]+1 and leaf <= can[3]:
					list_right_can.append(list1[leaf])
			"""
			if len(list_left_can) != 0 and len(list_right_can) != 0:
				#report here
				#we have list_left_can and list_right_can here, with tboth previous positions in
				for leaf1 in list_left_can:
					for leaf2 in list_right_can:
						if int(list1[leaf1]) < int(list1[leaf2]):
							name = str(list1[leaf1]) + "-" + str(list1[leaf2])
						else:
							name = str(list1[leaf2]) + "-" + str(list1[leaf1])
						hash_final[name] = [can[0], MRCA]
			"""
			if len(list_left_can) != 0 and len(list_right_can) != 0:
				result.append([list_left_can, list_right_can])
	return


def parser2(tree, h11, h2, h21, list1, list2, list3, result):  # deal with the second tree; begin the intersection operation
	# the following stack is local; should be released after this function
	stack = []

	# to get the actual start of a tree; start from "("
	n = len(tree)
	i = 0
	while tree[i] != "(":
		i+=1

	while i<n:
		if tree[i] == "," or tree[i] == " " or tree[i] == ";":
			i+=1
			continue

		if tree[i] == "(":
			i+=1
			stack.append(None)
			continue

		if tree[i].isdigit():
			s=""
			while tree[i]!="," and tree[i]!=")":
				s=s+tree[i]
				i=i+1
			data=NODE.split(s)
			node=int(DOT.split(data[0])[0])
			MRCA=float(data[1])

			# for the list and the stack:
			# the list is used to put all the leaves in-order, using their previous positions
			# the stack is usual as the previous one, assisting the stack operation
			list3.append(list2[node-1])

			if stack[-1] == None:
				stack[-1] = MRCA
				if (MRCA//1) in h2:
					#h2[MRCA//1].append([MRCA, [len(list3)-1], []])
					h2[MRCA//1].append([MRCA, [list2[node-1]], []])
				else:
					h2[MRCA//1] = [[MRCA, [list2[node-1]], []]]  # this list will be extended to a longer one
			else:
				h2[stack[-1]//1][-1][2] = [list2[node-1]]  # this list will be extended to a longer one
			continue

		if tree[i] == ")":
			MRCA = stack[-1]
			# perform the intersection from here
			if (MRCA//1) in h11 or (MRCA//1 + 1) in h11 or (MRCA//1 - 1) in h11:
				# here we should copy the list, because later we will regard them as candidate lists
				# ^ no worries, because we won't remove anything from the lists
				#list_left = h2[MRCA//1][-1][1][:]
				list_left = h2[MRCA//1][-1][1]
				#list_right = h2[MRCA//1][-1][2][:]
				list_right = h2[MRCA//1][-1][2]
				intersection(MRCA, list_left, list_right, h11, list1, result);
			else:
				pass

			# judge whether or not it is the end of the string
			if tree[i+1] != ";":
				# we can get a tP here
				i+=2
				s=""
				while tree[i]!="," and tree[i]!=")":
					s=s+tree[i]
					i=i+1
				tP=float(s)
				
				# get the updated tMRCA and the concatenated list
				list_temp = h2[stack[-1]//1][-1][1] + h2[stack[-1]//1][-1][2]
				
				# we don't care h21 any more because we are only testing this algorithm
				# no we should care, because h21 is used to store the final result, and h2 for stack
				list_temp1 = h2[stack[-1]//1].pop()
				if stack[-1]//1 in h21:
					h21[stack[-1]//1].append(list_temp1)
				else:
					h21[stack[-1]//1] = [list_temp1]

				MRCA = stack.pop() + tP
				# judge this is a left sub-tree or a right sub-tree
				if stack[-1] == None:
					# this is a left sub-tree
					stack[-1] = MRCA
					if (MRCA//1) in h2:
						h2[MRCA//1].append([MRCA, list_temp, []])
					else:
						h2[MRCA//1] = [[MRCA, list_temp, []]]
				else:
					# this is a right sub-tree
					h2[stack[-1]//1][-1][2] = list_temp

			else:
				# the finishing point, just continue; the bottom of the stack, should also perform intersection
				i+=1

				# remember to move up!!!
				list_temp1 = h2[stack[-1]//1].pop()
				# we don't care h21 any more because we are only testing this algorithm
				# no we should care about that
				if stack[-1]//1 in h21:
					h21[stack[-1]//1].append(list_temp1)
				else:
					h21[stack[-1]//1] = [list_temp1]
			continue
	return
