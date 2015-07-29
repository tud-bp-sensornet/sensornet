/*
 * Author: tud-bp-sensornet
 * 
 * Testfile that checks the correct working of the
 * k-hop algorithm in the 01-2hop-test.csc
 * Nodes have to print their Graph in the form of
 * "Testcase:Node:id,hop"
 * "Testcase:Edge:id,id"
 * after 5 minutes passed
 * 
 * To run the test with the simulation open Cooja and:
 * File->Open simulation->Open and Reconfigure->Browse...
 * Then choose which file should be compiled for the nodes
 */

/* Make test automatically fail (timeout) after 600 simulated seconds */
TIMEOUT(600000); /* milliseconds. no action at timeout */

/* Some variables */
moteDict = {};          //Associative Array that will hold messages of every node
moteAmount = 0;         //Amount of motes in the Simulation
testSuccess = false;    //Set to true if the testcondition is fulfilled
allmotes = [];

/* Check if the element value is contained in the array */
function arrayContains(array, value) {
  return array.indexOf(value) > -1;
}

/* Wait until node has booted */
WAIT_UNTIL(msg.startsWith('Starting'));

/* Fill the dictionary with the ID of every mote as keys and an empty Array as value*/
 allmotes = sim.getMotes();
 moteAmount = allmotes.length;
 for(var i = 0; i < moteAmount; i++){
      moteDict[allmotes[i].getID()] = [];
 }
 
/* Wait 300 seconds (300000ms) to build network */
GENERATE_MSG(300000, "continue");
YIELD_THEN_WAIT_UNTIL(msg.equals("continue"));

/* wait for building dict */
GENERATE_MSG(40000, "endloop");

/* Read Testcase messages */
while (!msg.equals("endloop")) {
  YIELD();
  //only save Testcase: messages
  if(msg.substr(0,9).equals("Testcase:")){
      //only save each message once
      if(arrayContains(moteDict[id], msg.substr(9,msg.length()-9)) == false){
            moteDict[id].push(msg.substr(9,msg.length()-9));
        }
  }
}

/* Check if test conditions are fulfilled */

testSuccess = 
    /* Node 1 */
    arrayContains(moteDict[1], "Node:1,0") &&
    arrayContains(moteDict[1], "Edge:1,2") &&
    arrayContains(moteDict[1], "Node:2,1") &&
    arrayContains(moteDict[1], "Edge:2,3") &&
    arrayContains(moteDict[1], "Edge:2,1") &&
    arrayContains(moteDict[1], "Node:3,2") &&
    arrayContains(moteDict[1], "Edge:3,2") &&
    arrayContains(moteDict[1], "Edge:3,4") &&
    arrayContains(moteDict[1], "Edge:3,5") &&
    arrayContains(moteDict[1], "Node:4,3") &&
    arrayContains(moteDict[1], "Node:5,3") &&
    (moteDict[1].length == 11) &&
    /* Node 2 */
    arrayContains(moteDict[2], "Node:2,0") &&
    arrayContains(moteDict[2], "Edge:2,1") &&
    arrayContains(moteDict[2], "Edge:2,3") &&
    arrayContains(moteDict[2], "Node:1,1") &&
    arrayContains(moteDict[2], "Edge:1,2") &&
    arrayContains(moteDict[2], "Node:3,1") &&
    arrayContains(moteDict[2], "Edge:3,2") &&
    arrayContains(moteDict[2], "Edge:3,4") &&
    arrayContains(moteDict[2], "Edge:3,5") &&
    arrayContains(moteDict[2], "Node:4,2") &&
    arrayContains(moteDict[2], "Edge:4,3") &&
    arrayContains(moteDict[2], "Node:5,2") &&
    arrayContains(moteDict[2], "Edge:5,3") &&
    arrayContains(moteDict[2], "Edge:5,6") &&
    arrayContains(moteDict[2], "Node:6,3") &&
    (moteDict[2].length == 15) &&
    /* Node 3 */
    arrayContains(moteDict[3], "Node:3,0") &&
    arrayContains(moteDict[3], "Edge:3,2") &&
    arrayContains(moteDict[3], "Edge:3,4") &&
    arrayContains(moteDict[3], "Edge:3,5") &&
    arrayContains(moteDict[3], "Node:2,1") &&
    arrayContains(moteDict[3], "Edge:2,1") &&
    arrayContains(moteDict[3], "Edge:2,3") &&
    arrayContains(moteDict[3], "Node:4,1") &&
    arrayContains(moteDict[3], "Edge:4,3") &&
    arrayContains(moteDict[3], "Node:5,1") &&
    arrayContains(moteDict[3], "Edge:5,3") &&
    arrayContains(moteDict[3], "Edge:5,6") &&
    arrayContains(moteDict[3], "Node:1,2") &&
    arrayContains(moteDict[3], "Edge:1,2") &&
    arrayContains(moteDict[3], "Node:6,2") &&
    arrayContains(moteDict[3], "Edge:6,5") &&
    (moteDict[3].length == 16) &&
    /* Node 4 */
    arrayContains(moteDict[4], "Node:4,0") &&
    arrayContains(moteDict[4], "Edge:4,3") &&
    arrayContains(moteDict[4], "Node:3,1") &&
    arrayContains(moteDict[4], "Edge:3,2") &&
    arrayContains(moteDict[4], "Edge:3,4") &&
    arrayContains(moteDict[4], "Edge:3,5") &&
    arrayContains(moteDict[4], "Node:2,2") &&
    arrayContains(moteDict[4], "Edge:2,3") &&
    arrayContains(moteDict[4], "Edge:2,1") &&
    arrayContains(moteDict[4], "Node:5,2") &&
    arrayContains(moteDict[4], "Edge:5,6") &&
    arrayContains(moteDict[4], "Edge:5,3") &&
    arrayContains(moteDict[4], "Node:6,3") &&
    arrayContains(moteDict[4], "Node:1,3") &&
    (moteDict[4].length == 14) &&
    /* Node 5 */
    arrayContains(moteDict[5], "Node:5,0") &&
    arrayContains(moteDict[5], "Edge:5,3") &&
    arrayContains(moteDict[5], "Edge:5,6") &&
    arrayContains(moteDict[5], "Node:6,1") &&
    arrayContains(moteDict[5], "Edge:6,5") &&
    arrayContains(moteDict[5], "Node:3,1") &&
    arrayContains(moteDict[5], "Edge:3,2") &&
    arrayContains(moteDict[5], "Edge:3,4") &&
    arrayContains(moteDict[5], "Edge:3,5") &&
    arrayContains(moteDict[5], "Node:2,2") &&
    arrayContains(moteDict[5], "Edge:2,3") &&
    arrayContains(moteDict[5], "Edge:2,1") &&
    arrayContains(moteDict[5], "Node:4,2") &&
    arrayContains(moteDict[5], "Edge:4,3") &&
    arrayContains(moteDict[5], "Node:1,3") &&
    (moteDict[5].length == 15) &&
    /* Node 6 */
    arrayContains(moteDict[6], "Node:6,0") &&
    arrayContains(moteDict[6], "Edge:6,5") &&
    arrayContains(moteDict[6], "Node:5,1") &&
    arrayContains(moteDict[6], "Edge:5,3") &&
    arrayContains(moteDict[6], "Edge:5,6") &&
    arrayContains(moteDict[6], "Node:3,2") &&
    arrayContains(moteDict[6], "Edge:3,2") &&
    arrayContains(moteDict[6], "Edge:3,4") &&
    arrayContains(moteDict[6], "Edge:3,5") &&
    arrayContains(moteDict[6], "Node:2,3") &&
    arrayContains(moteDict[6], "Node:4,3") &&
    (moteDict[6].length == 11)
    ;

if(testSuccess){
    log.testOK();
}

/* print */
for(var i = 1; i <= moteAmount; i++){
    for(var j = 0; j < moteDict[i].length; j++){
      log.log("Node: " + i + " Message: " + moteDict[i][j] + "\n");
      }
}
