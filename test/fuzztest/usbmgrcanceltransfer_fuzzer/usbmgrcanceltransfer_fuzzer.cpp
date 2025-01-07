/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "usbmgrcanceltransfer_fuzzer.h"

#include "usb_common_fuzz.h"
#include "usb_errors.h"

namespace OHOS {
const int32_t DEFAULT_ROLE_HOST = 1;
const int32_t BITS_PER_BYTE = 8;
const int32_t NUM_THREE = 3;
const int32_t NUM_TWO = 2;
const int32_t NUM_ONE = 1;
constexpr size_t THRESHOLD = 10;
namespace USB {
    uint32_t Convert2Uint32(const uint8_t *ptr)
    {
        if (ptr == nullptr) {
            return 0;
        }
        /*
        * Move the 0th digit 24 to the left, the first digit 16 to the left, the second digit 8 to the left,
        * and the third digit no left
        */
        return (ptr[0] << BITS_PER_BYTE * NUM_THREE) | (ptr[NUM_ONE] << BITS_PER_BYTE * NUM_TWO) |
            (ptr[NUM_TWO] << BITS_PER_BYTE) | (ptr[NUM_THREE]);
    }

    bool UsbMgrCancelTransferFuzzTest(const uint8_t* data, size_t /* size */)
    {
        if (data == nullptr) {
            return false;
        }
        uint32_t endPoint = Convert2Uint32(data);

        auto[res, pipe, interface] = UsbMgrPrepareFuzzEnv();
        if (!res) {
            USB_HILOGE(MODULE_USB_SERVICE, "prepare error");
            return false;
        }

        auto &usbSrvClient = UsbSrvClient::GetInstance();

        if (usbSrvClient.UsbCancelTransfer(const_cast<USBDevicePipe &>(pipe), endPoint)) {
            return false;
        }
        return true;
    }
} // USB
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::THRESHOLD) {
        return 0;
    }
    /* Run your code on data */
    OHOS::USB::UsbMgrCancelTransferFuzzTest(data, size);
    return 0;
}
