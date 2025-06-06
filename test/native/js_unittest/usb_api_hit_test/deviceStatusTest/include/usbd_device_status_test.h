/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef USBD_DEVICE_STATUS_TEST_H
#define USBD_DEVICE_STATUS_TEST_H

#include <gtest/gtest.h>
#include "UsbSubscriberTest.h"
#include "v2_0/iusbd_subscriber.h"
#include "v2_0/usb_types.h"

const int32_t USB_ENDPOINT_DIR_IN = 0x80;

using OHOS::HDI::Usb::V2_0::UsbDev;
namespace OHOS {
namespace USB {
class UsbdDeviceStatusTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static UsbDev dev_;
    static OHOS::sptr<OHOS::USB::UsbSubscriberTest> subscriber_;
};
} // USB
} // OHOS
#endif // USBD_MANAGE_INTERFACE_TEST_H