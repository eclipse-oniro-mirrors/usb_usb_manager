/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef SETTINGS_DATASHARE_HELPER_H
#define SETTINGS_DATASHARE_HELPER_H

#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "system_ability_definition.h"
#include "singleton.h"
#include "uri.h"

namespace OHOS {
namespace USB {
class UsbSettingDataShare {
public:
    UsbSettingDataShare();
    ~UsbSettingDataShare();
    bool Query(Uri& uri, const std::string& key, std::string& values);
private:
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int systemAbilityId);
    std::shared_ptr<DataShare::DataShareHelper> datashareHelper_ = nullptr;
};
} // namespace USB
} // namespace OHOS
#endif //SETTINGS_DATASHARE_HELPER_H