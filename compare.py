import os


file_shai = open(os.getcwd() + "/result_naive_test_1000G_50I_100Million.trees", "r")
file_shuo = open(os.getcwd() + "/result_verify_test_1000G_50I_100Million.trees", "r")

rep_shuo = {}
lines = file_shuo.readlines()
for line in lines:
	line = line.strip()
	rep_shuo[line] = 1

rep_shai = {}
lines = file_shai.readlines()
for line in lines:
	line = line.strip()
	rep_shai[line] = 1


print "Now testing... (start from Shuo's rep)"
for item in rep_shuo:
	if item in rep_shai:
		rep_shai[item] = 0
	else:
		print "Shuo has his own:",
		print item

print "Now let's see the rep of Shai:"
for item in rep_shai:
	if rep_shai[item] == 1:
		print "Shai has his own:",
		print item

print "testing done!"
