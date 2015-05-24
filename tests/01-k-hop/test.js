/*
 * Author: Marvin Haus
 * 
 * Testfile that checks the correct working of the
 * k-hop algorithm in the 01-2hop-test.csc
 * Nodes have to send messages in the form of
 * "Testcase:Node:id,hop,posX,posY"
 * "Testcase:Edge:id,id"
 * after 21 seconds passed
 * 
 * Open 01-2hop-test.csc in a text editor and change
 * <source EXPORT="discard">[CONTIKI_DIR]/examples/rime/example-broadcast.c</source>
 * <commands EXPORT="discard">make example-broadcast.sky TARGET=sky</commands>
 * <firmware EXPORT="copy">[CONTIKI_DIR]/examples/rime/example-broadcast.sky</firmware>
 * to the path of the correct file
 */

/* Make test automatically fail (timeout) after 120 simulated seconds */
TIMEOUT(120000); /* milliseconds. no action at timeout */

/* Some variables */
moteDict = {};          //Associative Array that will hold messages of every node
moteAmount = 0;         //Amount of motes in the Simulation
testSuccess = false;    //Set to true if the testcondition is fulfilled
allmotes = [];

/* Check if the element value is contained in the array */
function arrayContains(array, value) {
  return array.indexOf(value) > -1;
}

/* Calculate distance between 2 nodes with the IDs specified */
function calcDistance(id1, id2) {
    for(var i = 0; i < moteAmount; i++){
        if(allmotes[i].getID() == id1){
            mote1 = allmotes[i];
           }
        if(allmotes[i].getID() == id2){
            mote2 = allmotes[i];
           }
	}
    return mote1.getInterfaces().getPosition().getDistanceTo(mote2);
}

/* Wait until node has booted */
WAIT_UNTIL(msg.startsWith('Starting'));

/* Fill the dictionary with the ID of every mote as keys and an empty Array as value*/
 allmotes = sim.getMotes();
 moteAmount = allmotes.length;
 for(var i = 0; i < moteAmount; i++){
      moteDict[allmotes[i].getID()] = [];
 }
 
/* Wait 20 seconds (20000ms) to build network */
GENERATE_MSG(20000, "continue");
YIELD_THEN_WAIT_UNTIL(msg.equals("continue"));

/* wait for building dict */
GENERATE_MSG(20000, "endloop");

/* Read Testcase messages */
while (!msg.equals("endloop")) {
  YIELD();
  if(msg.substr(0,9).equals("Testcase:")){
    moteDict[id].push(msg.substr(10,msg.length()-9));
  }
}

/* Check if test conditions are fulfilled */

testSuccess = 
    /* Node 1 */
    arrayContains(moteDict[1], "Node:1,0,50,0") &&
    arrayContains(moteDict[1], "Edge:1,2") &&
    arrayContains(moteDict[1], "Node:2,1,90,0") &&
    arrayContains(moteDict[1], "Edge:2,3") &&
    arrayContains(moteDict[1], "Edge:2,1") &&
    arrayContains(moteDict[1], "Node:3,2,130,0") &&
    /* Node 2 */
    arrayContains(moteDict[2], "Node:2,0,90,0") &&
    arrayContains(moteDict[2], "Edge:2,1") &&
    arrayContains(moteDict[2], "Edge:2,3") &&
    arrayContains(moteDict[2], "Node:1,1,50,0") &&
    arrayContains(moteDict[2], "Edge:1,2") &&
    arrayContains(moteDict[2], "Node:3,1,130,0") &&
    arrayContains(moteDict[2], "Edge:3,2") &&
    arrayContains(moteDict[2], "Edge:3,4") &&
    arrayContains(moteDict[2], "Edge:3,5") &&
    arrayContains(moteDict[2], "Node:4,2,130,40") &&
    arrayContains(moteDict[2], "Node:5,2,130,-40") &&
    /* Node 3 */
    arrayContains(moteDict[3], "Node:3,0,130,0") &&
    arrayContains(moteDict[3], "Edge:3,2") &&
    arrayContains(moteDict[3], "Edge:3,4") &&
    arrayContains(moteDict[3], "Edge:3,5") &&
    arrayContains(moteDict[3], "Node:2,1,90,0") &&
    arrayContains(moteDict[3], "Edge:2,1") &&
    arrayContains(moteDict[3], "Node:4,1,130,40") &&
    arrayContains(moteDict[3], "Edge:4,3") &&
    arrayContains(moteDict[3], "Node:5,1,130,-40") &&
    arrayContains(moteDict[3], "Edge:5,3") &&
    arrayContains(moteDict[3], "Node:1,2,50,0") &&
    /* Node 4 */
    arrayContains(moteDict[4], "Node:4,0,130,40") &&
    arrayContains(moteDict[4], "Edge:4,3") &&
    arrayContains(moteDict[4], "Node:3,1,130,0") &&
    arrayContains(moteDict[4], "Edge:3,2") &&
    arrayContains(moteDict[4], "Edge:3,4") &&
    arrayContains(moteDict[4], "Edge:3,5") &&
    arrayContains(moteDict[4], "Node:2,2,90,0") &&
    arrayContains(moteDict[4], "Node:5,2,130,-40") &&
    /* Node 5 */
    arrayContains(moteDict[5], "Node:5,0,130,-40") &&
    arrayContains(moteDict[5], "Edge:5,3") &&
    arrayContains(moteDict[5], "Node:3,1,130,0") &&
    arrayContains(moteDict[5], "Edge:3,2") &&
    arrayContains(moteDict[5], "Edge:3,4") &&
    arrayContains(moteDict[5], "Edge:3,5") &&
    arrayContains(moteDict[5], "Node:2,2,90,0") &&
    arrayContains(moteDict[5], "Node:4,2,130,40")
    ;

/* iterate nodes */
//for(var i = 1; i <= moteAmount; i++){
//    /* iterate messages */
//    for(var j = 0; j < moteDict[i].length; j++){
//        /* iterate other nodes */
//        for(var k = 1; k <= moteAmount; k++){
//            /* check if the other mode is a neighbor (transmission range is 50) */
//            if(i != k && calcDistance(i,k) < 50){
//                   arrayContains(moteDict[k], moteDict[i][j]);
//                }
//        }
//    }
// }
/* Was the test a success?*/

if(testSuccess){
    log.testOK();
}

/* print */
for(var i = 1; i <= moteAmount; i++){
    for(var j = 0; j < moteDict[i].length; j++){
      log.log(moteDict[i][j] + "\n");
      }
}
