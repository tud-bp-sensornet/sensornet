#include "unit-test.h"
#include "contiki.h"
#include "positions.h"
#include <stdlib.h>

UNIT_TEST_REGISTER(stored_position_test, "Correct positions are returned for default positions");

UNIT_TEST(stored_position_test)
{
	rimeaddr_t addr;
	addr.u8[1] = 0;
	position_t pos;

	UNIT_TEST_BEGIN();

	addr.u8[0] = 1;
	pos = get_stored_position_of(&addr);
	UNIT_TEST_ASSERT(pos.x == 50 && pos.y == 0);

	addr.u8[0] = 2;	
	pos = get_stored_position_of(&addr);
	UNIT_TEST_ASSERT(pos.x == 90 && pos.y == 0);

	addr.u8[0] = 3;	
	pos = get_stored_position_of(&addr);
	UNIT_TEST_ASSERT(pos.x == 130 && pos.y == 0);

	addr.u8[0] = 4;	
	pos = get_stored_position_of(&addr);
	UNIT_TEST_ASSERT(pos.x == 130 && pos.y == 40);

	addr.u8[0] = 5;	
	pos = get_stored_position_of(&addr);
	UNIT_TEST_ASSERT(pos.x == 130 && pos.y == -40);

	addr.u8[0] = 6;
	pos = get_stored_position_of(&addr);
	UNIT_TEST_ASSERT(pos.x == 170 && pos.y == -40);

	UNIT_TEST_END();
}

int main()
{
	UNIT_TEST_RUN(stored_position_test);

	return EXIT_SUCCESS;
}
