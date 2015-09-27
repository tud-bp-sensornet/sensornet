#!/usr/bin/python

import sys, getopt
import xml.etree.ElementTree as etree

def printhelp():
	print('Tool for extracting node positions from .csc files.')
	print('Node IDs have to be in strict ascending order in the .csc file.')
	print('Node IDs should start with 1.')
	print('')
	print('Usage:')
	print('  position.py -i <inputfile>')
	print('')
	print('Options:')
	print('  -h --help                 Show this screen.')
	print('  -i --input=<inputfile>    The file to parse.')
	return

def main(argv):
	inputfile = ''
	string = 'NODE_POSITION_ARRAY = '
	idcheck = -1

	try:
		opts, args = getopt.getopt(argv,"hi:",["help", "input="])
	except getopt.GetoptError:
		printhelp()
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-h", "--help"):
			printhelp()
			sys.exit()
		elif opt in ("-i", "--input"):
			inputfile = arg
		else:
			printhelp()
			sys.exit()

	if(len(opts) == 0):
		printhelp()
		sys.exit()

	#parse XML into etree
	tree = etree.parse(inputfile)
	#get root
	root = tree.getroot()
	#find all mote nodes
	for mote in root.find('simulation').findall('mote'):
		conf = mote.findall('interface_config')

		if(len(conf) != 2):
			print('Input is not a valid csc file!')
			sys.exit(2)

		if(idcheck == -1):
			idcheck = int(conf[1].find('id').text)
			if(idcheck != 1):
				print('IDs should start with 1!')
				sys.exit(2)	
		elif(int(conf[1].find('id').text) == idcheck + 1):
			idcheck = idcheck + 1
		else:
			print('IDs are not in strict ascending order!')
			sys.exit(2)			

		if(conf[0].find('x').text[-2:] != '.0' or conf[0].find('y').text[-2:] != '.0'):
			print('#Attention: Decimal places were rounded: {' + conf[0].find('x').text + ',' + conf[0].find('y').text + '}')
		string += '{' + str(int(round(float(conf[0].find('x').text)))) + ',' + str(int(round(float(conf[0].find('y').text)))) + '},'

	print(string[:-1])
	print('NODE_POSITION_ARRAY_LENGTH = ' + str(len(root.find('simulation').findall('mote'))))
	print('POSITION_ARRAY_RIMEADDR_OFFSET = 1')

if __name__ == "__main__":
	main(sys.argv[1:])
