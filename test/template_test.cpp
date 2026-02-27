#include <gtest/gtest.h>
#include "template.h"

TEST(TemplateTest, Add) {
    int i = 10;
    int j = 20;
    say_hello();
    EXPECT_EQ(i+j,30);
}