CONTIKI="../../../../lib/contiki"
CSC="broadcast.simulation.csc"

if [ ! -f $CONTIKI/tools/cooja/dist/cooja.jar ]; then
	cd $CONTIKI/tools/cooja
	ant -q jar
	cd $OLDPWD
	
	#for travis
	while [ ! -f $CONTIKI/tools/cooja/dist/cooja.jar ]
	do
  		sleep 2
	done
fi

java -Xshare:on -jar $CONTIKI/tools/cooja/dist/cooja.jar -nogui=$CSC -contiki=$CONTIKI > /dev/null
cat COOJA.testlog

if grep -q "TEST FAILED" COOJA.testlog; then
	rm COOJA.testlog
	rm COOJA.log

	exit 1
fi

rm COOJA.testlog
rm COOJA.log
exit 0
