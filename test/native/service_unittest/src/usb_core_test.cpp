/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "usb_core_test.h"
#include <csignal>
#include <iostream>
#include <string>
#include <vector>

#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "usb_common_test.h"
#include "usb_errors.h"
#include "usb_srv_client.h"
#include "usb_srv_support.h"

using namespace testing::ext;
using namespace OHOS::USB;
using namespace OHOS;
using namespace std;
using namespace OHOS::USB::Common;

namespace OHOS {
namespace USB {
namespace Core {
#ifdef USB_MANAGER_FEATURE_DEVICE
constexpr int32_t USB_FUNCTION_INVALID = -1;
#endif // USB_MANAGER_FEATURE_DEVICE
constexpr int32_t USB_PORT_ID = 0;
constexpr int32_t USB_PORT_ID_INVALID = -1;
constexpr int32_t USB_POWER_ROLE_INVALID = -1;
constexpr int32_t USB_DATA_ROLE_INVALID = -1;

void UsbCoreTest::SetUpTestCase(void)
{
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Start UsbCoreTest");
}

void UsbCoreTest::TearDownTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "End UsbCoreTest");
}

void UsbCoreTest::SetUp(void) {}

void UsbCoreTest::TearDown(void) {}

#ifdef USB_MANAGER_FEATURE_DEVICE
/**
 * @tc.name: GetCurrentFunctions001
 * @tc.desc: Test functions to GetCurrentFunctions()
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, GetCurrentFunctions001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetCurrentFunctions001 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = static_cast<int32_t>(UsbSrvSupport::FUNCTION_NONE);
    int32_t ret = instance.GetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::ret=%{public}d", ret);
    ASSERT_EQ(0, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetCurrentFunctions001 : SetConfig");
}

/**
 * @tc.name: GetCurrentFunctions002
 * @tc.desc: Test functions to GetCurrentFunctions()
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, GetCurrentFunctions002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetCurrentFunctions002 : SetConfig");
    UsbCommonTest::GrantPermissionNormalNative();
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = static_cast<int32_t>(UsbSrvSupport::FUNCTION_NONE);
    int32_t ret = instance.GetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::ret=%{public}d", ret);
    ASSERT_NE(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetCurrentFunctions002 : SetConfig");
}

/**
 * @tc.name: GetCurrentFunctions003
 * @tc.desc: Test functions to GetCurrentFunctions()
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, GetCurrentFunctions003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetCurrentFunctions003 : SetConfig");
    UsbCommonTest::GrantNormalPermissionNative();
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = static_cast<int32_t>(UsbSrvSupport::FUNCTION_NONE);
    int32_t ret = instance.GetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::ret=%{public}d", ret);
    ASSERT_EQ(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetCurrentFunctions003 : SetConfig");
}

/**
 * @tc.name: GetCurrentFunctions004
 * @tc.desc: Test functions to GetCurrentFunctions()
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, GetCurrentFunctions004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetCurrentFunctions004 : SetConfig");
    UsbCommonTest::GrantSysNoPermissionNative();
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = static_cast<int32_t>(UsbSrvSupport::FUNCTION_NONE);
    int32_t ret = instance.GetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::ret=%{public}d", ret);
    ASSERT_NE(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetCurrentFunctions004 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions001
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions001 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t isok = instance.SetCurrentFunctions(UsbSrvSupport::FUNCTION_ACM);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions001 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions002
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions002 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t isok = instance.SetCurrentFunctions(UsbSrvSupport::FUNCTION_ECM);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions002 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions003
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions003 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = UsbSrvSupport::FUNCTION_ACM | UsbSrvSupport::FUNCTION_ECM;
    int32_t isok = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions003 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions004
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions004 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t isok = instance.SetCurrentFunctions(UsbSrvSupport::FUNCTION_HDC);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions004 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions005
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions005 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = UsbSrvSupport::FUNCTION_ACM | UsbSrvSupport::FUNCTION_HDC;
    int32_t isok = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions005 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions006
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions006 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = UsbSrvSupport::FUNCTION_ECM | UsbSrvSupport::FUNCTION_HDC;
    int32_t isok = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions006 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions007
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions007 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t isok = instance.SetCurrentFunctions(UsbSrvSupport::FUNCTION_MTP);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions007 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions008
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions008 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t isok = instance.SetCurrentFunctions(UsbSrvSupport::FUNCTION_PTP);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions008 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions009
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions009, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions009 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = UsbSrvSupport::FUNCTION_MTP | UsbSrvSupport::FUNCTION_HDC;
    int32_t isok = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions009 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions010
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions010, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions010 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = UsbSrvSupport::FUNCTION_PTP | UsbSrvSupport::FUNCTION_HDC;
    int32_t isok = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions010 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions011
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions011, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions011 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = UsbSrvSupport::FUNCTION_ACM | UsbSrvSupport::FUNCTION_MTP | UsbSrvSupport::FUNCTION_HDC;
    int32_t isok = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions011 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions012
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions012, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions012 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = UsbSrvSupport::FUNCTION_ACM | UsbSrvSupport::FUNCTION_PTP | UsbSrvSupport::FUNCTION_HDC;
    int32_t isok = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions012 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions013
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions013, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions013 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t isok = instance.SetCurrentFunctions(UsbSrvSupport::FUNCTION_NONE);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", isok);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "the function was set to none successfully");
    isok = instance.SetCurrentFunctions(UsbSrvSupport::FUNCTION_HDC);
    ASSERT_EQ(0, isok);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions013 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions014
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions014, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions014 : SetConfig");
    UsbCommonTest::GrantPermissionNormalNative();
    auto &instance = UsbSrvClient::GetInstance();
    int32_t ret = instance.SetCurrentFunctions(UsbSrvSupport::FUNCTION_HDC);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", ret);
    ASSERT_NE(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions014 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions015
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions015, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions015 : SetConfig");
    UsbCommonTest::GrantNormalPermissionNative();
    auto &instance = UsbSrvClient::GetInstance();
    int32_t ret = instance.SetCurrentFunctions(UsbSrvSupport::FUNCTION_HDC);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", ret);
    ASSERT_EQ(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions015 : SetConfig");
}

/**
 * @tc.name: SetCurrentFunctions016
 * @tc.desc: Test functions to SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetCurrentFunctions016, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions016 : SetConfig");
    UsbCommonTest::GrantSysNoPermissionNative();
    auto &instance = UsbSrvClient::GetInstance();
    int32_t ret = instance.SetCurrentFunctions(UsbSrvSupport::FUNCTION_HDC);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::SetCurrentFunctions=%{public}d", ret);
    ASSERT_NE(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions016 : SetConfig");
}

/**
 * @tc.name: UsbFunctionsFromString001
 * @tc.desc: Test functions to UsbFunctionsFromString(string funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, UsbFunctionsFromString001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsFromString001 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcCode = instance.UsbFunctionsFromString(UsbSrvSupport::FUNCTION_NAME_NONE);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::functionsFromString=%{public}d", funcCode);
    ASSERT_NE(UEC_SERVICE_INVALID_VALUE, funcCode);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsFromString001 : SetConfig");
}

/**
 * @tc.name: UsbFunctionsFromString002
 * @tc.desc: Test functions to UsbFunctionsFromString(string funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, UsbFunctionsFromString002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsFromString002 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcCode = instance.UsbFunctionsFromString(UsbSrvSupport::FUNCTION_NAME_HDC);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::functionsFromString=%{public}d", funcCode);
    ASSERT_NE(UEC_SERVICE_INVALID_VALUE, funcCode);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsFromString002 : SetConfig");
}

/**
 * @tc.name: UsbFunctionsFromString003
 * @tc.desc: Test functions to UsbFunctionsFromString(string funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, UsbFunctionsFromString003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsFromString003 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcCode = instance.UsbFunctionsFromString(UsbSrvSupport::FUNCTION_NAME_ACM);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::functionsFromString=%{public}d", funcCode);
    ASSERT_NE(UEC_SERVICE_INVALID_VALUE, funcCode);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsFromString003 : SetConfig");
}

/**
 * @tc.name: UsbFunctionsFromString004
 * @tc.desc: Test functions to UsbFunctionsFromString(string funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, UsbFunctionsFromString004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsFromString004 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcCode = instance.UsbFunctionsFromString(UsbSrvSupport::FUNCTION_NAME_ECM);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::functionsFromString=%{public}d", funcCode);
    ASSERT_NE(UEC_SERVICE_INVALID_VALUE, funcCode);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsFromString004 : SetConfig");
}

/**
 * @tc.name: UsbFunctionsFromString005
 * @tc.desc: Test functions to UsbFunctionsFromString(string funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, UsbFunctionsFromString005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsFromString005 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "qwerts";
    int32_t funcCode = instance.UsbFunctionsFromString(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::functionsFromString=%{public}d", funcCode);
    ASSERT_EQ(UEC_SERVICE_INVALID_VALUE, funcCode);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsFromString005 : SetConfig");
}

/**
 * @tc.name: UsbFunctionsFromString006
 * @tc.desc: Test functions to UsbFunctionsFromString(string funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, UsbFunctionsFromString006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsFromString006 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "zxcbvx";
    int32_t funcCode = instance.UsbFunctionsFromString(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::functionsFromString=%{public}d", funcCode);
    ASSERT_EQ(UEC_SERVICE_INVALID_VALUE, funcCode);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsFromString006 : SetConfig");
}

/**
 * @tc.name: UsbFunctionsToString001
 * @tc.desc: Test functions to UsbFunctionsToString(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, UsbFunctionsToString001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsToString001 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcName = instance.UsbFunctionsToString(UsbSrvSupport::FUNCTION_NONE);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::UsbFunctionsToString=%{public}s", funcName.c_str());
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::UsbFunctionsToString=%{public}zu", funcName.size());
    ASSERT_TRUE(!(funcName.empty()));
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsToString001 : SetConfig");
}

/**
 * @tc.name: UsbFunctionsToString002
 * @tc.desc: Test functions to UsbFunctionsToString(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, UsbFunctionsToString002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsToString002 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcName = instance.UsbFunctionsToString(UsbSrvSupport::FUNCTION_HDC);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::UsbFunctionsToString=%{public}s", funcName.c_str());
    ASSERT_TRUE(!(funcName.empty()));
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsToString002 : SetConfig");
}

/**
 * @tc.name: UsbFunctionsToString003
 * @tc.desc: Test functions to UsbFunctionsToString(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, UsbFunctionsToString003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsToString003 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcName = instance.UsbFunctionsToString(UsbSrvSupport::FUNCTION_ACM);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbFunctionServiceTest::UsbFunctionsToString=%{public}s", funcName.c_str());
    ASSERT_TRUE(!(funcName.empty()));
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsToString003 : SetConfig");
}

/**
 * @tc.name: UsbFunctionsToString004
 * @tc.desc: Test functions to UsbFunctionsToString(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, UsbFunctionsToString004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsToString004 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcName = instance.UsbFunctionsToString(UsbSrvSupport::FUNCTION_ECM);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbFunctionServiceTest::UsbFunctionsToString=%{public}s", funcName.c_str());
    ASSERT_TRUE(!(funcName.empty()));
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsToString004 : SetConfig");
}

/**
 * @tc.name: UsbFunctionsToString005
 * @tc.desc: Test functions to UsbFunctionsToString(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, UsbFunctionsToString005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsToString005 : SetConfig");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcName = instance.UsbFunctionsToString(USB_FUNCTION_INVALID);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbFunctionServiceTest::UsbFunctionsToString=%{public}s", funcName.c_str());
    ASSERT_TRUE(!(funcName.empty()));
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsToString005 : SetConfig");
}
#endif // USB_MANAGER_FEATURE_DEVICE

/**
 * @tc.name: GetPorts001
 * @tc.desc: Test functions to GetPorts
 * @tc.desc: int32_t GetPorts(std::vector<UsbPort *> &usbports);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, GetPorts001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetPorts001 : GetPorts");
    auto &usbSrvClient = UsbSrvClient::GetInstance();
    std::vector<UsbPort> portlist;
    auto ports = usbSrvClient.GetPorts(portlist);
    USB_HILOGD(MODULE_USB_SERVICE, "Get UsbPort size=%{public}zu", portlist.size());
    ASSERT_NE(ports, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetPorts001 : GetPorts");
}

/**
 * @tc.name: GetSupportedModes001
 * @tc.desc: Test functions to GetSupportedModes
 * @tc.desc: int32_t GetSupportedModes(int32_t portId, int32_t &result);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */

HWTEST_F(UsbCoreTest, GetSupportedModes001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetSupportedModes001 : GetSupportedModes");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    int32_t result;
    auto modes = UsbSrvClient.GetSupportedModes(UsbSrvSupport::PORT_MODE_NONE, result);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::status=%{public}d", modes);
    ASSERT_NE(modes, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetSupportedModes001 : GetSupportedModes");
}

/**
 * @tc.name: GetSupportedModes002
 * @tc.desc: Test functions to GetSupportedModes
 * @tc.desc: int32_t GetSupportedModes(int32_t portId, int32_t &result);
 * @tc.desc: 反向测试：portid错误
 * @tc.type: FUNC
 */

HWTEST_F(UsbCoreTest, GetSupportedModes002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetSupportedModes002 : GetSupportedModes");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    int32_t result;
    auto modes = UsbSrvClient.GetSupportedModes(USB_PORT_ID_INVALID, result);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::status=%{public}d", modes);
    ASSERT_NE(modes, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetSupportedModes002 : GetSupportedModes");
}

/**
 * @tc.name: GetSupportedModes003
 * @tc.desc: Test functions to GetSupportedModes
 * @tc.desc: int32_t GetSupportedModes(int32_t portId, int32_t &result);
 * @tc.desc: 反向测试：portid错误
 * @tc.type: FUNC
 */

HWTEST_F(UsbCoreTest, GetSupportedModes003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetSupportedModes003 : GetSupportedModes");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    int32_t result = 0;
    auto modes = UsbSrvClient.GetSupportedModes(0xFFFFFFFF, result); // 0xFFFFFFFF:Maximum anomaly portId
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::status=%{public}d", modes);
    ASSERT_NE(modes, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetSupportedModes003 : GetSupportedModes");
}

/**
 * @tc.name: GetSupportedModes004
 * @tc.desc: Test functions to GetSupportedModes
 * @tc.desc: int32_t GetSupportedModes(int32_t portId, int32_t &result);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */

HWTEST_F(UsbCoreTest, GetSupportedModes004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetSupportedModes004 : GetSupportedModes");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    int32_t result;
    auto modes = UsbSrvClient.GetSupportedModes(USB_PORT_ID, result);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::status=%{public}d", modes);
    ASSERT_NE(0, modes);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetSupportedModes004 : GetSupportedModes");
}

/**
 * @tc.name: SetPortRole001
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */

HWTEST_F(UsbCoreTest, SetPortRole001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole001 : SetPortRole");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.SetPortRole(USB_PORT_ID, UsbSrvSupport::POWER_ROLE_SOURCE, UsbSrvSupport::DATA_ROLE_HOST);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::status=%{public}d", ret);
    ret = UsbCommonTest::SwitchErrCode(ret);
    ASSERT_NE(0, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole001 : SetPortRole");
}

/**
 * @tc.name: SetPortRole002
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole);
 * @tc.desc: 反向测试：portid错误
 * @tc.type: FUNC
 */

HWTEST_F(UsbCoreTest, SetPortRole002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole002 : SetPortRole");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.SetPortRole(
        USB_PORT_ID_INVALID, UsbSrvSupport::POWER_ROLE_SOURCE, UsbSrvSupport::DATA_ROLE_HOST);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::status=%{public}d", ret);
    ret = UsbCommonTest::SwitchErrCode(ret);
    ASSERT_NE(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole002 : SetPortRole");
}

/**
 * @tc.name: SetPortRole003
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole);
 * @tc.desc: 反向测试：powerRole错误
 * @tc.type: FUNC
 */

HWTEST_F(UsbCoreTest, SetPortRole003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole003 : SetPortRole");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.SetPortRole(
        USB_PORT_ID, USB_POWER_ROLE_INVALID, UsbSrvSupport::DATA_ROLE_DEVICE);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::status=%{public}d", ret);
    ret = UsbCommonTest::SwitchErrCode(ret);
    ASSERT_NE(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole003 : SetPortRole");
}

/**
 * @tc.name: SetPortRole004
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole);
 * @tc.desc: 反向测试：dataRole错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetPortRole004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole004 : SetPortRole");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.SetPortRole(
        USB_PORT_ID, UsbSrvSupport::POWER_ROLE_SOURCE, USB_DATA_ROLE_INVALID);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::status=%{public}d", ret);
    ret = UsbCommonTest::SwitchErrCode(ret);
    ASSERT_NE(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole004 : SetPortRole");
}

/**
 * @tc.name: SetPortRole005
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole);
 * @tc.desc: 反向测试：参数异常，portId、powerRole错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetPortRole005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole005 : SetPortRole");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.SetPortRole(USB_PORT_ID_INVALID, USB_POWER_ROLE_INVALID, USB_DATA_ROLE_INVALID);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::status=%{public}d", ret);
    ret = UsbCommonTest::SwitchErrCode(ret);
    ASSERT_NE(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole005 : SetPortRole");
}

/**
 * @tc.name: SetPortRole006
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole);
 * @tc.desc: 反向测试：powerRole、dataRole错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetPortRole006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole006 : SetPortRole");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.SetPortRole(USB_PORT_ID, USB_POWER_ROLE_INVALID, USB_DATA_ROLE_INVALID);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::status=%{public}d", ret);
    ret = UsbCommonTest::SwitchErrCode(ret);
    ASSERT_NE(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole006 : SetPortRole");
}

/**
 * @tc.name: SetPortRole007
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole);
 * @tc.desc: 反向测试：powerRole、dataRole错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetPortRole007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole007 : SetPortRole");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.SetPortRole(USB_PORT_ID, USB_POWER_ROLE_INVALID, USB_DATA_ROLE_INVALID);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::status=%{public}d", ret);
    ret = UsbCommonTest::SwitchErrCode(ret);
    ASSERT_NE(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole007 : SetPortRole");
}

/**
 * @tc.name: SetPortRole008
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbCoreTest, SetPortRole008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole008 : SetPortRole");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.SetPortRole(
        USB_PORT_ID, UsbSrvSupport::DATA_ROLE_DEVICE, UsbSrvSupport::POWER_ROLE_SINK);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::status=%{public}d", ret);
    ret = UsbCommonTest::SwitchErrCode(ret);
    ASSERT_NE(0, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole008 : SetPortRole");
}
} // Core
} // USB
} // OHOS
