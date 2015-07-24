/*
 * Author: tud-bp-sensornet
 * 
 * Testfile that checks the correct working of broadcasts
 * 
 */

TIMEOUT(60000);

mote_outputs = {}; // Associative Array that will hold messages of every node
test_success = true;

/* Check if the element value is contained in the array */
function array_contains(array, value)
{
	//return array.indexOf(value) > -1; // this doesn't work for some reason
	var found = false;
	array.forEach(function(e)
	{
		if (e == value) found = true;    
	}
	);
	return found;
}

function assert_mote_output(id, message)
{
	if (!array_contains(mote_outputs[id], message))
	{
		log.log("Assert failed: mote " + id + " has not printed expected message '" + message + "'\n");
		test_success = false;
	}
}

function print_all_outputs()
{
	sim.getMotes().forEach(function(mote)
	{
		log.log("## Outputs of mote " + mote.getID() + ":\n");
		mote_outputs[mote.getID()].forEach(function (output)
		{
			log.log("* " + output + "\n");
		}
		);
	}
	);
}

/* Wait until node has booted */
WAIT_UNTIL(msg.startsWith('Starting'));

/* Fill the dictionary with the ID of every mote as keys and an empty array as value */
sim.getMotes().forEach(function(mote)
{
	mote_outputs[mote.getID()] = [];
}
);

/* wait for building dict */
GENERATE_MSG(50000, "end");

/* Collect all messages */
while (!msg.equals("end"))
{
	YIELD();

	mote_outputs[id].push(msg);
}

/* Check if test conditions are fulfilled */
assert_mote_output(1, "BEGIN SEND CORRECT PACKET");
assert_mote_output(1, "[Broadcast] hash: 5027 datalen: 66");
assert_mote_output(1, "END SEND CORRECT PACKET");
assert_mote_output(1, "BEGIN SEND FAULTY PACKET");
assert_mote_output(1, "END SEND FAULTY PACKET");

assert_mote_output(2, "BROADCAST RECEIVED WITH CORRECT HASH, SIZE=64, FIRST BYTE=32");
assert_mote_output(2, "broadcast received from 1.0 (size 64): 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F ");
assert_mote_output(2, "[Broadcast] Incorrect hash (expected 20519, but is 7069); discarding received packet.");

assert_mote_output(3, "BROADCAST RECEIVED WITH CORRECT HASH, SIZE=64, FIRST BYTE=32");
assert_mote_output(3, "broadcast received from 1.0 (size 64): 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F ");
assert_mote_output(3, "[Broadcast] Incorrect hash (expected 20519, but is 7069); discarding received packet.");

if (test_success) log.testOK(); else log.testFailed();
