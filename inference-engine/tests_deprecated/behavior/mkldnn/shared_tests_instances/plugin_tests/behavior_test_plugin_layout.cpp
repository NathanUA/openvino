// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "behavior_test_plugin_layout.hpp"

layout_test_params power_test_cases[] = {
    layout_test_params(CommonTestUtils::DEVICE_CPU, "FP32", Layout::C, power_params({ { 3 } }, 2, 2, 2)),
    layout_test_params(CommonTestUtils::DEVICE_CPU, "FP32", Layout::NC, power_params({ { 1, 3 } }, 2, 2, 2)),
    layout_test_params(CommonTestUtils::DEVICE_CPU, "FP32", Layout::CHW, power_params({ { 3, 32, 16 } }, 2, 2, 2)),
    layout_test_params(CommonTestUtils::DEVICE_CPU, "FP32", Layout::NCHW, power_params({ { 1, 3, 16, 16 } }, 2, 2, 2)),
};

layout_test_params conv_test_cases[] = {
    layout_test_params(CommonTestUtils::DEVICE_CPU, "FP32", Layout::NCHW, power_params({ { 1, 3, 16, 16 } }, 2, 2, 2)),
};

layout_test_params conv_neg_test_cases[] = {
    layout_test_params(CommonTestUtils::DEVICE_CPU, "FP32", Layout::C, power_params({ { 3 } }, 2, 2, 2)),
    layout_test_params(CommonTestUtils::DEVICE_CPU, "FP32", Layout::NC, power_params({ { 1, 3 } }, 2, 2, 2)),
    layout_test_params(CommonTestUtils::DEVICE_CPU, "FP32", Layout::CHW, power_params({ { 3, 32, 16 } }, 2, 2, 2)),
};

INSTANTIATE_TEST_CASE_P(BehaviorTest, LayoutTestCanLoadPower,
    ::testing::ValuesIn(power_test_cases), getTestName);
INSTANTIATE_TEST_CASE_P(BehaviorTest, LayoutTestCanLoadConv,
    ::testing::ValuesIn(conv_test_cases), getTestName);
INSTANTIATE_TEST_CASE_P(BehaviorTest, LayoutTestCanNotLoadConv,
    ::testing::ValuesIn(conv_neg_test_cases), getTestName);