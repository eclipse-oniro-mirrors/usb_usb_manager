/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "usbmgrrequestright_fuzzer.h"

#include "usb_srv_client.h"
#include "usb_errors.h"

namespace OHOS {
namespace USB {
    bool UsbMgrRequestRightFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size == 0 || data[size-1] != 0) {
            USB_HILOGE(MODULE_USB_SERVICE, "data size is insufficient!");
            return false;
        }
        auto &usbSrvClient = UsbSrvClient::GetInstance();
        if (usbSrvClient.RequestRight(std::string(reinterpret_cast<const char*>(data))) == UEC_OK) {
            return false;
        }
        return true;
    }
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::USB::UsbMgrRequestRightFuzzTest(data, size);
    return 0;
}

