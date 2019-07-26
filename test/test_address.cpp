/*
*  Copyright (C) Ivan Ryabov - All Rights Reserved
*
*  Unauthorized copying of this file, via any medium is strictly prohibited.
*  Proprietary and confidential.
*
*  Written by Ivan Ryabov <abbyssoul@gmail.com>
*/
/*******************************************************************************
 * ghostd Unit Test Suit
 *	@file test/test_address.cpp
 *	@brief		Test suit for tribe::Address
 ******************************************************************************/
#include "tribe/networkAddress.hpp"    // Class being tested.

#include <gtest/gtest.h>

using namespace tribe;


TEST(TestAddress, testParsing_ipv4) {
	EXPECT_TRUE(tryParseAddress("238.255.0.1:5670").isOk());
	EXPECT_TRUE(tryParseAddress("255.255.255.255:5670").isOk());

	EXPECT_FALSE(tryParseAddress("0.0.0.1").isOk());
	EXPECT_FALSE(tryParseAddress("10.1.1.1:56705670").isOk());
	EXPECT_FALSE(tryParseAddress("some-rangom long string ?!").isOk());
	EXPECT_FALSE(tryParseAddress("8761:0.0.0.1").isOk());
	EXPECT_FALSE(tryParseAddress("32.x.0.1:5670").isOk());
	EXPECT_FALSE(tryParseAddress("32.x.0.1:5670:321").isOk());
}


TEST(TestAddress, testParsing_ipv6) {
	EXPECT_TRUE(tryParseAddress("[ff02::1]:5678").isOk());
	EXPECT_TRUE(tryParseAddress("[2001:db8:85a3:8d3:1319:8a2e:370:7348]:5670").isOk());
	EXPECT_TRUE(tryParseAddress("[fe80::27ae:adff:dfa1:743e]:5670").isOk());

	EXPECT_FALSE(tryParseAddress("[fe80::27ae:adff:dfa1:743e]:1235670").isOk());
	EXPECT_FALSE(tryParseAddress("[fe80::27ae:adff:dfa1:743e]:235670:327").isOk());
	EXPECT_FALSE(tryParseAddress("fe80::27ae:adff:dfa1:743e").isOk());
	EXPECT_FALSE(tryParseAddress("fe80::27ae:adff:dfa1:743e").isOk());
	EXPECT_FALSE(tryParseAddress("[ff02::1678").isOk());
	EXPECT_FALSE(tryParseAddress("fe80::27ae:adff:dfa1:743e:fe80::27ae:adff").isOk());
}
