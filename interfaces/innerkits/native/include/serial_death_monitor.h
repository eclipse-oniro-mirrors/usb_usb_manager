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

#ifndef SERIAL_DEATH_MONITOR_H
#define SERIAL_DEATH_MONITOR_H

#include "ipc_object_stub.h"

namespace OHOS::USB {
class SerialDeathMonitor : public OHOS::IPCObjectStub {
public:
    explicit SerialDeathMonitor() : OHOS::IPCObjectStub() {}
    ~SerialDeathMonitor() override = default;
    int32_t OnRemoteRequest(uint32_t code, OHOS::MessageParcel &data, OHOS::MessageParcel &reply,
        OHOS::MessageOption &option)override
        {
            return 0;
        }
};
} // namespace OHOS::USB
#endif // SERIAL_DEATH_MONITOR_H
