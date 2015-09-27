/*
 * Author: tud-bp-sensornet
 * 
 * Testfile that checks the correct working of the
 * k-hop algorithm in the 2hop-test.csc (with purging)
 * Nodes have to print their Graph in the form of
 * "Testcase:Node:id,hop"
 * "Testcase:Edge:id,id"
 * after 115 seconds passed
 */

/* Make test automatically fail (timeout) after 600 simulated seconds */
TIMEOUT(200000); /* milliseconds. no action at timeout */

/* Some variables */
mote_dict = {};          //Associative Array that will hold messages of every node
mote_amount = 0;         //Amount of motes in the Simulation
test_success = true;     //Set to false if the testcondition isn't fulfilled
all_motes = [];

/* Check if the element value is contained in the array */
function array_contains(array, value)
{
	return array.indexOf(value) > -1;
}

/* Check if the element value is not contained in the array */
function assert_mote_output(id, message)
{
	if (!array_contains(mote_dict[id], message))
	{
		log.log("Assert failed: mote " + id + " has not printed expected message '" + message + "'\n");
		test_success = false;
	}
}

/* Check if the array length with the id has the specified length */
function assert_mote_array_length(id, array_length)
{
	if (mote_dict[id].length != array_length)
	{
		log.log("Assert failed: mote " + id + " has not printed expected message '" + array_length + "'\n");
		test_success = false;
	}
}

/* Wait until node has booted */
WAIT_UNTIL(msg.startsWith('Starting'));

/* Fill the dictionary with the ID of every mote as keys and an empty Array as value*/
all_motes = sim.getMotes();
mote_amount = all_motes.length;
for(var i = 0; i < mote_amount; i++)
{
	mote_dict[all_motes[i].getID()] = [];
}
 
/* Wait 40 seconds (40000ms) to build network*/
// After 40 seconds the network is completely build (see 2-hop-test) 
GENERATE_MSG(40000, "continue");
YIELD_THEN_WAIT_UNTIL(msg.equals("continue"));

//remove one node
sim.removeMote(sim.getMoteWithID(1));

/* Wait 70 seconds (70000ms) to purge network*/
// After 70 seconds every edge (to/from 1) should have a ttl of 0 AND min. 1 purge was made 
GENERATE_MSG(70000, "continue");
YIELD_THEN_WAIT_UNTIL(msg.equals("continue"));

/* wait for building dict */
GENERATE_MSG(10000, "endloop");

/* Read Testcase messages */
while (!msg.equals("endloop"))
{
	YIELD();
	//only save Testcase: messages
	if (msg.substr(0,9).equals("Testcase:"))
	{
		//only save each message once
		if(array_contains(mote_dict[id], msg.substr(9,msg.length()-9)) == false){
			mote_dict[id].push(msg.substr(9,msg.length()-9));
		}
	}
}

/* Check if test conditions are fulfilled */
/* Node 1 */
assert_mote_array_length(1, 0);

/* Node 2 */
assert_mote_output(2, "Node:2,0");
assert_mote_output(2, "Edge:2,3");
assert_mote_output(2, "Node:3,1");
assert_mote_output(2, "Edge:3,2");
assert_mote_output(2, "Edge:3,4");
assert_mote_output(2, "Edge:3,5");
assert_mote_output(2, "Node:4,2");
assert_mote_output(2, "Edge:4,3");
assert_mote_output(2, "Node:5,2");
assert_mote_output(2, "Edge:5,3");
assert_mote_array_length(2, 10);

/* Node 3 */
assert_mote_output(3, "Node:3,0");
assert_mote_output(3, "Edge:3,2");
assert_mote_output(3, "Edge:3,4");
assert_mote_output(3, "Edge:3,5");
assert_mote_output(3, "Node:2,1");
assert_mote_output(3, "Edge:2,3");
assert_mote_output(3, "Node:4,1");
assert_mote_output(3, "Edge:4,3");
assert_mote_output(3, "Node:5,1");
assert_mote_output(3, "Edge:5,3");
assert_mote_array_length(3, 10);

/* Node 4 */
assert_mote_output(4, "Node:4,0");
assert_mote_output(4, "Edge:4,3");
assert_mote_output(4, "Node:3,1");
assert_mote_output(4, "Edge:3,2");
assert_mote_output(4, "Edge:3,4");
assert_mote_output(4, "Edge:3,5");
assert_mote_output(4, "Node:2,2");
assert_mote_output(4, "Edge:2,3");
assert_mote_output(4, "Node:5,2");
assert_mote_output(4, "Edge:5,3");
assert_mote_array_length(4, 10);

/* Node 5 */
assert_mote_output(5, "Node:5,0");
assert_mote_output(5, "Edge:5,3");
assert_mote_output(5, "Node:3,1");
assert_mote_output(5, "Edge:3,2");
assert_mote_output(5, "Edge:3,4");
assert_mote_output(5, "Edge:3,5");
assert_mote_output(5, "Node:2,2");
assert_mote_output(5, "Edge:2,3");
assert_mote_output(5, "Node:4,2");
assert_mote_output(5, "Edge:4,3");
assert_mote_array_length(5, 10);

if (test_success)
{
	log.testOK();
}

/* print */
for (var i = 1; i <= mote_amount; i++)
{
	for (var j = 0; j < mote_dict[i].length; j++)
	{
		log.log("Node: " + i + " Message: " + mote_dict[i][j] + "\n");
	}
}
