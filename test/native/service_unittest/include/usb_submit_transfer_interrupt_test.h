/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef USB_REQUEST_INTERRUPT_TEST_H
#define USB_REQUEST_INTERRUPT_TEST_H

#include <gtest/gtest.h>

namespace OHOS {
namespace USB {
namespace SubmitTransfer {
class UsbSubmitTransferInterruptTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
static constexpr int32_t FIVE = -5;
static constexpr int32_t TEN = 10;
static constexpr int32_t TYPE_BULK = 2;
static constexpr int32_t TYPE_INTERRUPT = 3;

enum UsbInterruptTestErrCode {
    OHEC_COMMON_PARAM_ERROR = 401,
    USB_SUBMIT_TRANSFER_IO_ERROR = 14400007,
    USB_SUBMIT_TRANSFER_NO_DEVICE_ERROR =  14400009,
    USB_SUBMIT_TRANSFER_TIMEOUT_ERROR = 14400010,
    USB_SUBMIT_TRANSFER_OVERFLOW_ERROR = 14400012,
    USB_SUBMIT_TRANSFER_NOT_FOUND_ERROR = 14400015,
};

} // SubmitTransfer
} // USB
} // OHOS
#endif // USB_REQUEST_INTERRUPT_TEST_H
