CONTIKI="../../../../lib/contiki"
CSC="broadcast.simulation.csc"

java -Xshare:on -jar $CONTIKI/tools/cooja/dist/cooja.jar -nogui=$CSC -contiki=$CONTIKI > /dev/null
cat COOJA.testlog
rm COOJA.testlog
rm COOJA.log