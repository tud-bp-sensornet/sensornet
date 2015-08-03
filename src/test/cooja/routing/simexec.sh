CONTIKI="../../../../lib/contiki"
CSC="routing.csc"

if [ ! -f $CONTIKI/tools/cooja/dist/cooja.jar ]; then
	cd $CONTIKI/tools/cooja
	ant -q jar
	cd $OLDPWD
fi

java -Xshare:auto -jar $CONTIKI/tools/cooja/dist/cooja.jar -nogui=$CSC -contiki=$CONTIKI > /dev/null
cat COOJA.testlog

if grep -q "TEST OK" COOJA.testlog; then
	rm COOJA.testlog
	rm COOJA.log
	exit 0
fi

rm COOJA.testlog
rm COOJA.log
exit 1
