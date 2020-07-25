//
// Created by Stack on 7/25/20.
//

#define BOOST_TEST_MODULE Suite_example
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TwoTwoFour_suite)
    BOOST_AUTO_TEST_CASE(testPlus) {
        BOOST_CHECK_EQUAL(2+2, 4);
    }
    BOOST_AUTO_TEST_CASE(testMult) {
        BOOST_CHECK_EQUAL(2*2, 4);
    }
BOOST_AUTO_TEST_SUITE_END()