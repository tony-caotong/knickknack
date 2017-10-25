#! /bin/env python2

import sys

def main(args):
	if len(args) != 2:
		return -1
	f = open(args[1], 'r')
	content = None
	length = 0
	size = 0
	while (True):
		content = f.read(1024*1024*10)
		length = len(content)
		if (length == 0):
			break;

		size += length;
	f.close()
	
	print "Size: ", size


if __name__ == '__main__':
	r = main(sys.argv)
	sys.exit(r)
