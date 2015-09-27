/*
 * Author: tud-bp-sensornet
 * 
 * Testfile that checks the correctness of the
 * routing unicast.
 * Test will take approx. 50 seconds but max. 70.
 */

/* Make test automatically fail (timeout) after 70 simulated seconds */
TIMEOUT(70000); /* milliseconds. no action at timeout */

/* Some variables */
message_count = 0;
test_success = true;     //Set to false if the testcondition isn't fulfilled

/* Wait until node has booted */
WAIT_UNTIL(msg.startsWith('Starting'));

/* Wait 40 seconds (40000ms) to build network */
GENERATE_MSG(40000, "continue");
YIELD_THEN_WAIT_UNTIL(msg.equals("continue"));

//Press button aka. sensor 
sim.getMoteWithID(1).getInterfaces().getButton().clickButton();

/* wait for unicast to reach target */
GENERATE_MSG(10000, "endloop");

/* Read Testcase messages */
while (!msg.equals("endloop"))
{
	YIELD();

	if (msg.startsWith("[routing.c] get_nearest_neighbour:"))
	{
		continue;
	}

	message_count++;

	switch (message_count)
	{
	case 1:
		if (!(msg.equals("[routing.c] Will send initial unicast to: 2") && id == 1))
		{
			test_success = false;
			log.log("1st message was wrong!\n");
		}
		break;
	case 2:
		if (!(msg.equals("[routing.c] Got unicast from: 1.0 Content: Fire!") && id == 2))
		{
			test_success = false;
			log.log("2nd message was wrong!\n");
		}
		break;
	case 3:
		if (!(msg.equals("[routing.c] Will forward to: 3.0") && id == 2))
		{
			test_success = false;
			log.log("3rd message was wrong!\n");
		}
		break;
	case 4:
		if (!(msg.equals("[routing.c] Got unicast from: 2.0 Content: Fire!") && id == 3))
		{
			test_success = false;
			log.log("4th message was wrong!\n");
		}
		break;
	case 5:
		if (!(msg.equals("[routing.c] Will forward to: 4.0") && id == 3))
		{
			test_success = false;
			log.log("5th message was wrong!\n");
		}
		break;
	case 6:
		if (!(msg.equals("[routing.c] Got unicast from: 3.0 Content: Fire!") && id == 4))
		{
			test_success = false;
			log.log("6th message was wrong!\n");
		}
		break;
	case 7:
		if (!(msg.equals("[routing.c] Got it!") && id == 4))
		{
			test_success = false;
			log.log("7th message was wrong!\n");
		}
		break;
	case 8:
		if (!(msg.equals("endloop")))
		{
			test_success = false;
			log.log("Extra message!\n");
		}
		break;
	default:
		test_success = false;
		log.log("Extra message!\n");
	}
}

if (test_success)
{
	log.testOK();
}

