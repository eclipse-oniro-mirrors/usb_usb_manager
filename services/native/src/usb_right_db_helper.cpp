/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "usb_right_db_helper.h"

#include "bundle_installer_interface.h"
#include "hilog_wrapper.h"
#include "usb_errors.h"
#include "usb_right_database.h"
#include <string>

using namespace OHOS::NativeRdb;

namespace OHOS {
namespace USB {

std::shared_ptr<UsbRightDbHelper> UsbRightDbHelper::instance_;

UsbRightDbHelper::UsbRightDbHelper()
{
    rightDatabase_ = UsbRightDataBase::GetInstance();
}

std::shared_ptr<UsbRightDbHelper> UsbRightDbHelper::GetInstance()
{
    if (instance_ == nullptr) {
        instance_.reset(new UsbRightDbHelper());
    }
    return instance_;
}

bool UsbRightDbHelper::IsRecordExpired(
    int32_t uid, const std::string &deviceName, const std::string &bundleName, uint64_t expiredTime)
{
    std::vector<struct UsbRightAppInfo> infos;
    int32_t ret = QueryRightRecord(uid, deviceName, bundleName, infos);
    if (ret <= 0) {
        USB_HILOGE(MODULE_USB_SERVICE, "query no record/error: %{public}d", ret);
        return true;
    }
    USB_HILOGD(MODULE_USB_SERVICE, "query count: %{public}d", ret);
    size_t len = infos.size();
    for (size_t i = 0; i < len; i++) {
        struct UsbRightAppInfo info = infos.at(i);
        if (!IsRecordExpired(info, expiredTime)) {
            return false;
        }
    }
    return true;
}

bool UsbRightDbHelper::IsRecordExpired(struct UsbRightAppInfo &info, uint64_t expiredTime)
{
    USB_HILOGD(MODULE_USB_SERVICE,
        "info: inst=%{public}" PRIu64 " updt=%{public}" PRIu64 " rqst=%{public}" PRIu64 " expr=%{public}" PRIu64
        " valid=%{public}" PRIu64 "",
        info.installTime, info.updateTime, info.requestTime, expiredTime, info.validPeriod);
    if (info.validPeriod == USB_RIGHT_VALID_PERIOD_MIN) {
        USB_HILOGD(MODULE_USB_SERVICE, "allow temporary");
        return false;
    } else if (info.validPeriod == USB_RIGHT_VALID_PERIOD_MAX) {
        USB_HILOGD(MODULE_USB_SERVICE, "allow forever");
        return false;
    } else if (info.requestTime + info.validPeriod > expiredTime) {
        USB_HILOGD(MODULE_USB_SERVICE, "allow based on request time");
        return false;
    } else if ((info.installTime > info.updateTime) || (info.installTime > info.requestTime)) {
        USB_HILOGW(MODULE_USB_SERVICE,
            "invalid: install=%{public}" PRIu64 " update=%{public}" PRIu64 " request=%{public}" PRIu64 "",
            info.installTime, info.updateTime, info.requestTime);
        /* ignore, return true to add right */
        return true;
    }
    /* unknown, return true to add right */
    return true;
}

int32_t UsbRightDbHelper::AddRightRecord(
    const std::string &deviceName, const std::string &bundleName, struct UsbRightAppInfo &info)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    ValuesBucket values;
    values.Clear();
    values.PutInt("uid", info.uid);
    values.PutLong("installTime", info.installTime);
    values.PutLong("updateTime", info.updateTime);
    values.PutLong("requestTime", info.requestTime);
    values.PutLong("validPeriod", info.validPeriod);
    values.PutString("deviceName", deviceName);
    values.PutString("bundleName", bundleName);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }
    ret = rightDatabase_->Insert(values);
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Insert error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    USB_HILOGI(MODULE_USB_SERVICE,
        "add success: uid=%{public}d installTime=%{public}" PRIu64 " updateTime=%{public}" PRIu64
        " requestTime=%{public}" PRIu64 " validPeriod=%{public}" PRIu64 " deviceName=%{public}s bundleName=%{public}s",
        info.uid, info.installTime, info.updateTime, info.requestTime, info.validPeriod, deviceName.c_str(),
        bundleName.c_str());
    return ret;
}

int32_t UsbRightDbHelper::QueryRightRecord(int32_t uid, const std::string &deviceName, const std::string &bundleName,
    std::vector<struct UsbRightAppInfo> &infos)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    USB_HILOGD(MODULE_USB_SERVICE, "Query detail: uid=%{public}d deviceName=%{public}s bundleName=%{public}s", uid,
        deviceName.c_str(), bundleName.c_str());
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    std::vector<std::string> columns;
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.BeginWrap()
        ->EqualTo("uid", std::to_string(uid))
        ->And()
        ->EqualTo("deviceName", deviceName)
        ->And()
        ->EqualTo("bundleName", bundleName)
        ->EndWrap();
    std::unique_ptr<AbsSharedResultSet> resultSet = rightDatabase_->Query(rdbPredicates, columns);
    if (resultSet == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Query error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    return GetResultRightRecordEx(resultSet, infos);
}

int32_t UsbRightDbHelper::QueryUserRightRecord(int32_t uid, std::vector<struct UsbRightAppInfo> &infos)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    USB_HILOGD(MODULE_USB_SERVICE, "Query detail: uid=%{public}d", uid);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    std::vector<std::string> columns;
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.EqualTo("uid", std::to_string(uid));
    std::unique_ptr<AbsSharedResultSet> resultSet = rightDatabase_->Query(rdbPredicates, columns);
    if (resultSet == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Query error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    return GetResultRightRecordEx(resultSet, infos);
}

int32_t UsbRightDbHelper::QueryDeviceRightRecord(
    int32_t uid, const std::string &deviceName, std::vector<struct UsbRightAppInfo> &infos)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    USB_HILOGD(MODULE_USB_SERVICE, "Query detail: uid=%{public}d deviceName=%{public}s", uid, deviceName.c_str());
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    std::vector<std::string> columns;
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.BeginWrap()->EqualTo("uid", std::to_string(uid))->And()->EqualTo("deviceName", deviceName)->EndWrap();
    std::unique_ptr<AbsSharedResultSet> resultSet = rightDatabase_->Query(rdbPredicates, columns);
    if (resultSet == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Query error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    return GetResultRightRecordEx(resultSet, infos);
}

int32_t UsbRightDbHelper::QueryAppRightRecord(
    int32_t uid, const std::string &bundleName, std::vector<struct UsbRightAppInfo> &infos)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    USB_HILOGD(MODULE_USB_SERVICE, "Query detail: uid=%{public}d bundleName=%{public}s", uid, bundleName.c_str());
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    std::vector<std::string> columns;
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.BeginWrap()->EqualTo("uid", std::to_string(uid))->And()->EqualTo("bundleName", bundleName)->EndWrap();
    std::unique_ptr<AbsSharedResultSet> resultSet = rightDatabase_->Query(rdbPredicates, columns);
    if (resultSet == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Query error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    return GetResultRightRecordEx(resultSet, infos);
}

int32_t UsbRightDbHelper::QueryRightRecordUids(std::vector<std::string> &uids)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    std::vector<std::string> columns = {"uid"};
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.Distinct();
    std::unique_ptr<AbsSharedResultSet> resultSet = rightDatabase_->Query(rdbPredicates, columns);

    if (resultSet == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "QuerySql error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }

    struct UsbRightTableInfo table;
    if (GetResultSetTableInfo(resultSet, table) != USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetRowCount or GetColumnCount or uid index error");
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }

    bool endFlag = false;
    int32_t position = 0;
    for (int32_t i = 0; (i < table.rowCount) && !endFlag; i++) {
        if (resultSet->GoToRow(i) != E_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "GoToRow %{public}d", i);
            break;
        }
        std::string tempStr;
        if (resultSet->GetString(table.uidIndex, tempStr) == E_OK) {
            uids.push_back(tempStr);
        }
        resultSet->IsEnded(endFlag);
    }
    resultSet->GetRowIndex(position);
    resultSet->IsEnded(endFlag);
    USB_HILOGD(MODULE_USB_SERVICE, "row=%{public}d col=%{public}d pos=%{public}d ret=%{public}d end=%{public}s",
        table.rowCount, table.columnCount, position, uids.size(), (endFlag ? "yes" : "no"));
    return uids.size();
}

int32_t UsbRightDbHelper::QueryRightRecordApps(int32_t uid, std::vector<std::string> &apps)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    std::vector<std::string> columns = {"bundleName"};
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.EqualTo("uid", std::to_string(uid))->Distinct();
    std::unique_ptr<AbsSharedResultSet> resultSet = rightDatabase_->Query(rdbPredicates, columns);

    if (resultSet == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "QuerySql error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }

    struct UsbRightTableInfo table;
    if (GetResultSetTableInfo(resultSet, table) != USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetRowCount or GetColumnCount or bundleName index error");
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }

    bool endFlag = false;
    for (int32_t i = 0; (i < table.rowCount) && !endFlag; i++) {
        if (resultSet->GoToRow(i) != E_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "GoToRow %{public}d", i);
            break;
        }
        std::string tempStr;
        if (resultSet->GetString(table.bundleNameIndex, tempStr) == E_OK) {
            apps.push_back(tempStr);
        }
        resultSet->IsEnded(endFlag);
    }
    int32_t position = 0;
    resultSet->GetRowIndex(position);
    resultSet->IsEnded(endFlag);
    USB_HILOGD(MODULE_USB_SERVICE, "row=%{public}d col=%{public}d pos=%{public}d ret=%{public}d end=%{public}s",
        table.rowCount, table.columnCount, position, apps.size(), (endFlag ? "yes" : "no"));
    return apps.size();
}

int32_t UsbRightDbHelper::UpdateRightRecord(
    int32_t uid, const std::string &deviceName, const std::string &bundleName, struct UsbRightAppInfo &info)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    ValuesBucket values;
    values.Clear();
    values.PutInt("uid", info.uid);
    values.PutLong("installTime", info.installTime);
    values.PutLong("updateTime", info.updateTime);
    values.PutLong("requestTime", info.requestTime);
    values.PutLong("validPeriod", info.validPeriod);
    values.PutString("deviceName", deviceName);
    values.PutString("bundleName", bundleName);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    int32_t changedRows = 0;
    ret = rightDatabase_->Update(changedRows, values, "uid = ? AND deviceName = ? AND bundleName = ?",
        std::vector<std::string> {std::to_string(info.uid), deviceName, bundleName});
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Update error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    USB_HILOGD(
        MODULE_USB_SERVICE, "UPDATE table SET key1=value1,... WHERE uid = ? AND deviceName = ? AND bundleName = ?");
    return ret;
}

int32_t UsbRightDbHelper::DeleteRightRecord(int32_t uid, const std::string &deviceName, const std::string &bundleName)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }
    int32_t changedRows = 0;
    ret = rightDatabase_->Delete(changedRows, "uid = ? AND deviceName = ? AND bundleName = ?",
        std::vector<std::string> {std::to_string(uid), deviceName, bundleName});
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbRightDbHelper Delete error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    USB_HILOGD(MODULE_USB_SERVICE, "detale from table where uid = ? AND deviceName = ? AND bundleName = ?");
    return ret;
}

int32_t UsbRightDbHelper::DeleteDeviceRightRecord(int32_t uid, const std::string &deviceName)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    int32_t changedRows = 0;
    ret = rightDatabase_->Delete(
        changedRows, "uid = ? AND deviceName = ?", std::vector<std::string> {std::to_string(uid), deviceName});
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Delete error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }

    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    USB_HILOGD(MODULE_USB_SERVICE, "delete from table where uid = ? AND deviceName = ?");
    return ret;
}

int32_t UsbRightDbHelper::DeleteAppRightRecord(int32_t uid, const std::string &bundleName)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    int32_t changedRows = 0;
    ret = rightDatabase_->Delete(
        changedRows, "uid = ? AND bundleName = ?", std::vector<std::string> {std::to_string(uid), bundleName});
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbRightDbHelper Delete error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }

    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    USB_HILOGD(MODULE_USB_SERVICE, "delete from table where uid = ? AND bundleName = ?");
    return ret;
}

int32_t UsbRightDbHelper::DeleteUidRightRecord(int32_t uid)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    int32_t changedRows = 0;
    ret = rightDatabase_->Delete(changedRows, "uid = ?", std::vector<std::string> {std::to_string(uid)});
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbRightDbHelper Delete error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }

    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    USB_HILOGD(MODULE_USB_SERVICE, "delete from table where uid = ?");
    return ret;
}

int32_t UsbRightDbHelper::DeleteNormalExpiredRightRecord(int32_t uid, long expiredTime)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    int32_t changedRows = 0;
    ret = rightDatabase_->Delete(changedRows, "uid = ? AND requestTime < ? AND validPeriod NOT IN (?, ?)",
        std::vector<std::string> {std::to_string(uid), std::to_string(expiredTime - USB_RIGHT_VALID_PERIOD_SET),
            std::to_string(USB_RIGHT_VALID_PERIOD_MIN), std::to_string(USB_RIGHT_VALID_PERIOD_MAX)});
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbRightDbHelper Delete error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }

    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    USB_HILOGI(
        MODULE_USB_SERVICE, "delete from table where uid = %{public}d AND expiredTime = %{public}ld", uid, expiredTime);
    return ret;
}

int32_t UsbRightDbHelper::DeleteValidPeriodRightRecord(long validPeriod, const std::string &deviceName)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    int32_t changedRows = 0;
    ret = rightDatabase_->Delete(changedRows, "validPeriod = ? AND deviceName = ?",
        std::vector<std::string> {std::to_string(validPeriod), deviceName});
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbRightDbHelper Delete error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }

    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    USB_HILOGD(MODULE_USB_SERVICE, "delete from table where uid = ? AND validPeriod = ?");
    return ret;
}

int32_t UsbRightDbHelper::GetResultSetTableInfo(
    const std::unique_ptr<OHOS::NativeRdb::AbsSharedResultSet> &resultSet, struct UsbRightTableInfo &table)
{
    int32_t rowCount = 0;
    int32_t columnCount = 0;
    if (resultSet->GetRowCount(rowCount) != E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetRowCount error");
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    if (resultSet->GetColumnCount(columnCount) != E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetColumnCount error");
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    std::vector<std::string> columnNames;
    if (resultSet->GetAllColumnNames(columnNames) != E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetAllColumnNames error");
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    int32_t columnNamesCount = columnNames.size();
    for (int32_t i = 0; i < columnNamesCount; i++) {
        std::string columnName = columnNames.at(i);
        table.primaryKeyIndex = (columnName == "id" ? i : 0);
        table.uidIndex = (columnName == "uid" ? i : 0);
        table.installTimeIndex = (columnName == "installTime" ? i : 0);
        table.updateTimeIndex = (columnName == "updateTime" ? i : 0);
        table.requestTimeIndex = (columnName == "requestTime" ? i : 0);
        table.validPeriodIndex = (columnName == "validPeriod" ? i : 0);
        table.bundleNameIndex = (columnName == "bundleName" ? i : 0);
        table.deviceNameIndex = (columnName == "deviceName" ? i : 0);
    }
    table.rowCount = rowCount;
    table.columnCount = columnCount;
    USB_HILOGD(MODULE_USB_SERVICE,
        "info: row=%{public}d col=%{public}d idIdx=%{public}d uidIdx=%{public}d inIdx=%{public}d upIdx=%{public}d "
        "reIdx=%{public}d vaIdx=%{public}d devIdx=%{public}d bunIdx=%{public}d",
        rowCount, columnCount, table.primaryKeyIndex, table.uidIndex, table.installTimeIndex, table.updateTimeIndex,
        table.requestTimeIndex, table.validPeriodIndex, table.deviceNameIndex, table.bundleNameIndex);
    return USB_RIGHT_OK;
}

int32_t UsbRightDbHelper::GetResultRightRecordEx(
    const std::unique_ptr<OHOS::NativeRdb::AbsSharedResultSet> &resultSet, std::vector<struct UsbRightAppInfo> &infos)
{
    struct UsbRightTableInfo table;
    int32_t ret = GetResultSetTableInfo(resultSet, table);
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetResultSetTableInfo failed");
        return ret;
    }
    bool endFlag = false;
    std::string primaryKeyStr, uidStr, installTimeStr, updateTimeStr, requestTimeStr, validPeriodStr;
    for (int32_t i = 0; (i < table.rowCount) && !endFlag; i++) {
        if (resultSet->GoToRow(i) != E_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "GoToRow %{public}d", i);
            break;
        }
        if (resultSet->GetString(table.primaryKeyIndex, primaryKeyStr) == E_OK &&
            resultSet->GetString(table.uidIndex, uidStr) == E_OK &&
            resultSet->GetString(table.installTimeIndex, installTimeStr) == E_OK &&
            resultSet->GetString(table.updateTimeIndex, updateTimeStr) == E_OK &&
            resultSet->GetString(table.requestTimeIndex, requestTimeStr) == E_OK &&
            resultSet->GetString(table.validPeriodIndex, validPeriodStr) == E_OK) {
            struct UsbRightAppInfo info;
            info.primaryKeyId = std::stol(primaryKeyStr);
            info.uid = std::stol(uidStr);
            info.installTime = std::stoull(installTimeStr);
            info.updateTime = std::stoull(updateTimeStr);
            info.requestTime = std::stoull(requestTimeStr);
            info.validPeriod = std::stoull(validPeriodStr);
            infos.push_back(info);
        }
        resultSet->IsEnded(endFlag);
    }
    int32_t position = 0;
    resultSet->GetRowIndex(position);
    resultSet->IsEnded(endFlag);
    USB_HILOGD(MODULE_USB_SERVICE, "row=%{public}d col=%{public}d pos=%{public}d ret=%{public}d end=%{public}s",
        table.rowCount, table.columnCount, position, infos.size(), (endFlag ? "yes" : "no"));
    return infos.size();
}

int32_t UsbRightDbHelper::AddOrUpdateRightRecord(
    int32_t uid, const std::string &deviceName, const std::string &bundleName, struct UsbRightAppInfo &info)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }

    bool isUpdate = false;
    ret = CheckIfNeedUpdateEx(isUpdate, uid, deviceName, bundleName);
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "check if need update error: %{public}d", ret);
        return ret;
    }
    ret = AddOrUpdateRightRecordEx(isUpdate, uid, deviceName, bundleName, info);
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "add or update error: %{public}d", ret);
        return ret;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
    }
    return ret;
}

int32_t UsbRightDbHelper::CheckIfNeedUpdateEx(
    bool &isUpdate, int32_t uid, const std::string &deviceName, const std::string &bundleName)
{
    std::vector<std::string> columns;
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.BeginWrap()
        ->EqualTo("uid", std::to_string(uid))
        ->And()
        ->EqualTo("deviceName", deviceName)
        ->And()
        ->EqualTo("bundleName", bundleName)
        ->EndWrap();
    std::unique_ptr<AbsSharedResultSet> resultSet = rightDatabase_->Query(rdbPredicates, columns);
    if (resultSet == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Query error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    int32_t rowCount = 0;
    if (resultSet->GetRowCount(rowCount) != E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetRowCount error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    isUpdate = (rowCount > 0 ? true : false);
    return USB_RIGHT_OK;
}

int32_t UsbRightDbHelper::AddOrUpdateRightRecordEx(bool isUpdate, int32_t uid, const std::string &deviceName,
    const std::string &bundleName, struct UsbRightAppInfo &info)
{
    USB_HILOGD(MODULE_USB_SERVICE, "info: uid=%{public}d deviceName=%{public}s bundleName=%{public}s", uid,
        deviceName.c_str(), bundleName.c_str());
    int32_t ret = 0;
    ValuesBucket values;
    values.Clear();
    values.PutInt("uid", info.uid);
    values.PutLong("installTime", info.installTime);
    values.PutLong("updateTime", info.updateTime);
    values.PutLong("requestTime", info.requestTime);
    values.PutLong("validPeriod", info.validPeriod);
    values.PutString("deviceName", deviceName);
    values.PutString("bundleName", bundleName);
    if (isUpdate) {
        USB_HILOGI(MODULE_USB_SERVICE, " UPDATE table SET key1=value1,... WHERE ...");
        int32_t changedRows = 0;
        ret = rightDatabase_->Update(changedRows, values, "uid = ? AND deviceName = ? AND bundleName = ?",
            std::vector<std::string> {std::to_string(info.uid), deviceName, bundleName});
        if (ret < USB_RIGHT_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "Update error :%{public}d", ret);
            (void)rightDatabase_->RollBack();
        }
    } else {
        USB_HILOGI(MODULE_USB_SERVICE, "INSERT INTO table ...");
        ret = rightDatabase_->Insert(values);
        if (ret < USB_RIGHT_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "Insert error :%{public}d", ret);
            (void)rightDatabase_->RollBack();
        }
    }
    return ret;
}

int32_t UsbRightDbHelper::QueryRightRecordCount()
{
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }
    std::vector<std::string> columns;
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    std::unique_ptr<AbsSharedResultSet> resultSet = rightDatabase_->Query(rdbPredicates, columns);
    if (resultSet == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Query error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error :%{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    int32_t rowCount = 0;
    if (resultSet->GetRowCount(rowCount) != E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetRowCount error");
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    return rowCount;
}

} // namespace USB
} // namespace OHOS