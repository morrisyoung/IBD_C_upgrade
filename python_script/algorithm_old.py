#!/usr/bin/python
# function: the old caldurpar algorithm


import re

#TREE_WHOLE=re.compile(r'^tree\s(.*)')
#TREE_NUMBER_BREAKPOINT=re.compile(r'_(\d+)_(\d+)_pos_(\d+)\s')
#TREE_CONTENT=re.compile(r'\((.*)\);')
NODE=re.compile(r':')
DOT=re.compile(r'\.')
#END="100000000"  # we should manually set this
SAMPLE = 10
TOLERANCE = 0.01


def cal_during_par(tree, htable):  # a simple and subtle parser
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
