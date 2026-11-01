#include <criterion/criterion.h>
#include "app.hpp"
Test(app, addition){ cr_assert_eq(add(2,3),5); }
