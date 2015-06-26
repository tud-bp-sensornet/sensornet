#include "contiki.h"
#include "unit-test.h"

/* Register example unit tests that will be executed by using
   the UNIT_TEST_RUN macro. */
UNIT_TEST_REGISTER(arithmetic, "Arith ops");

/* arithmetic: Demonstrates a test that succeeds. The exit point will be 
   the line where UNIT_TEST_END is called. */
UNIT_TEST(arithmetic)
        {
                int a, b;

        UNIT_TEST_BEGIN();

        a = 1;
        b = 2;

        UNIT_TEST_ASSERT(a + b == 3);

        UNIT_TEST_END();
        }

void main() {
    UNIT_TEST_RUN(arithmetic);
}