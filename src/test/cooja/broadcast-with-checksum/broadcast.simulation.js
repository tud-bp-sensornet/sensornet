/*
 * Author: tud-bp-sensornet
 * 
 * Testfile that checks the correct working of broadcasts
 * 
 */

TIMEOUT(60000);

moteOutputs = {}; // Associative Array that will hold messages of every node
testSuccess = true;

/* Check if the element value is contained in the array */
function arrayContains(array, value) {
	//return array.indexOf(value) > -1; // this doesn't work for some reason
    var found = false;
    array.forEach(function(e) {
        if (e == value) found = true;    
    });
    return found;
}

function assertMoteOutput(id, message) {
	if (!arrayContains(moteOutputs[id], message)) {
		log.log("Assert failed: mote " + id +  " has not printed expected message '" + message + "'\n");
		testSuccess = false;
	}
}

function print_all_outputs() {
	sim.getMotes().forEach(function(mote) {
	    log.log("## Outputs of mote " + mote.getID() + ":\n");
	    moteOutputs[mote.getID()].forEach(function (output) {
	        log.log("* " + output + "\n");
	    });
	});
}

/* Wait until node has booted */
WAIT_UNTIL(msg.startsWith('Starting'));

/* Fill the dictionary with the ID of every mote as keys and an empty array as value */
sim.getMotes().forEach(function(mote) {
    moteOutputs[mote.getID()] = [];
});

/* wait for building dict */
GENERATE_MSG(50000, "end");

/* Collect all messages */
while (!msg.equals("end")) {
	YIELD();

	moteOutputs[id].push(msg);
}

/* Check if test conditions are fulfilled */
assertMoteOutput(1, "BEGIN SEND CORRECT PACKET");
assertMoteOutput(1, "[Broadcast] hash: 5027 datalen: 66");
assertMoteOutput(1, "END SEND CORRECT PACKET");
assertMoteOutput(1, "BEGIN SEND FAULTY PACKET");
assertMoteOutput(1, "END SEND FAULTY PACKET");

assertMoteOutput(2, "BROADCAST RECEIVED WITH CORRECT HASH, SIZE=64, FIRST BYTE=32");
assertMoteOutput(2, "broadcast received from 1.0 (size 64): 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F ");
assertMoteOutput(2, "[Broadcast] Incorrect hash (expected 20519, but is 7069); discarding received packet.");

assertMoteOutput(3, "BROADCAST RECEIVED WITH CORRECT HASH, SIZE=64, FIRST BYTE=32");
assertMoteOutput(3, "broadcast received from 1.0 (size 64): 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F ");
assertMoteOutput(3, "[Broadcast] Incorrect hash (expected 20519, but is 7069); discarding received packet.");

if (testSuccess) log.testOK(); else log.testFailed();
