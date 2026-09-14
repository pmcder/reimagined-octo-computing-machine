#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

static void test_placeholder_pipeline_wiring(void) {
    /* Proves the Unity + CTest wiring works end to end; replaced by real
     * test cases once the suite is fleshed out. */
    TEST_ASSERT_TRUE(1);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_placeholder_pipeline_wiring);
    return UNITY_END();
}
