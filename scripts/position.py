#!/usr/bin/python

import sys, getopt
import xml.etree.ElementTree as etree

def main(argv):
	inputfile = ''
	string = 'NODE_POSITION_ARRAY = '
	try:
		opts, args = getopt.getopt(argv,"hi:",["help", "input="])
	except getopt.GetoptError:
		print 'Usage: position.py -i <inputfile>'
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-h", "--help"):
			print 'Usage: position.py -i <inputfile>'
			sys.exit()
		elif opt in ("-i", "--input"):
			inputfile = arg

	#parse XML into etree
	tree = etree.parse(inputfile)
	#get root
	root = tree.getroot()
	#find all mote nodes
	for mote in root.find('simulation').findall('mote'):
		conf = mote.find('interface_config')
		if(conf.find('x').text[-2:] != '.0' or conf.find('y').text[-2:] != '.0'):
			print('#Attention: Decimal places were rounded: {' + conf.find('x').text + ',' + conf.find('y').text + '}')
		string += '{' + str(int(round(float(conf.find('x').text)))) + ',' + str(int(round(float(conf.find('y').text)))) + '},'

	print(string[:-1])
	print('NODE_POSITION_ARRAY_LENGTH = ' + str(len(root.find('simulation').findall('mote'))))

if __name__ == "__main__":
	main(sys.argv[1:])
