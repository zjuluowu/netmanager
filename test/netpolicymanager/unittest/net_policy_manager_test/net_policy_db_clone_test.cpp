/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "net_policy_db_clone.h"

#include <gtest/gtest.h>
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "net_policy_rule.h"
#include "net_bundle_impl.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "net_access_policy_rdb.h"
#define private public

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
namespace {
sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy()
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        return nullptr;
    }

    auto remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        return nullptr;
    }

    sptr<AppExecFwk::IBundleMgr> iBundleMgr = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (iBundleMgr == nullptr) {
        return nullptr;
    }
    return iBundleMgr;
}
constexpr const char* POLICY_DATABASE_BACKUP_FILE_TEST =
    "/data/service/el1/public/netmanager/net_uid_access_policy_backup_test.txt";
constexpr int32_t MAIN_USER_ID = 100;
}
class NetPolicyDBCloneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetPolicyDBCloneTest::SetUpTestCase() {}

void NetPolicyDBCloneTest::TearDownTestCase() {}

void NetPolicyDBCloneTest::SetUp() {}

void NetPolicyDBCloneTest::TearDown() {}

HWTEST_F(NetPolicyDBCloneTest, OnBackupTest001, TestSize.Level1)
{
    auto bundleMgrProxy = GetBundleMgrProxy();
    ASSERT_NE(bundleMgrProxy, nullptr);
    NetAccessPolicyRDB netAccessPolicyRdb;
    int32_t uid1 = bundleMgrProxy->GetUidByBundleName("com.ss.hm.artical.news", MAIN_USER_ID);
    NetAccessPolicyData policyData1;
    policyData1.wifiPolicy = 1;
    policyData1.cellularPolicy = 0;
    policyData1.setFromConfigFlag = 1;
    policyData1.uid = uid1;
    netAccessPolicyRdb.InsertData(policyData1);
    int32_t uid2 = bundleMgrProxy->GetUidByBundleName("com.youku.next", MAIN_USER_ID);
    NetAccessPolicyData policyData2;
    policyData2.wifiPolicy = 0;
    policyData2.cellularPolicy = 1;
    policyData2.setFromConfigFlag = 1;
    policyData2.uid = uid2;
    netAccessPolicyRdb.InsertData(policyData2);
    int32_t uid3 = bundleMgrProxy->GetUidByBundleName("com.xingin.xhs_hos", MAIN_USER_ID);
    NetAccessPolicyData policyData3;
    policyData3.wifiPolicy = 0;
    policyData3.cellularPolicy = 1;
    policyData3.setFromConfigFlag = 1;
    policyData3.uid = uid3;
    netAccessPolicyRdb.InsertData(policyData3);
    int32_t uid4 = bundleMgrProxy->GetUidByBundleName("com.ctrip.harmonynext", MAIN_USER_ID);
    NetAccessPolicyData policyData4;
    policyData4.wifiPolicy = 1;
    policyData4.cellularPolicy = 1;
    policyData4.setFromConfigFlag = 1;
    policyData4.uid = uid4;
    netAccessPolicyRdb.InsertData(policyData4);
    NetAccessPolicyData policyData5;
    policyData5.wifiPolicy = 1;
    policyData5.cellularPolicy = 1;
    policyData5.setFromConfigFlag = 1;
    policyData5.uid = 156161616;
    netAccessPolicyRdb.InsertData(policyData5);

    NetPolicyDBClone netpolicyClone;
    UniqueFd fd1(-1);
    netpolicyClone.OnBackup(fd1, "");
    EXPECT_NE(fd1, 1);
    close(fd1.Release());
}

HWTEST_F(NetPolicyDBCloneTest, OnRestoreTest001, TestSize.Level1)
{
    std::string content;
    std::ostringstream ss;
    auto bundleMgrProxy = GetBundleMgrProxy();
    ASSERT_NE(bundleMgrProxy, nullptr);
    ss << "com.xingin.xhs_hos" << " " << "0" << " " << "1" << std::endl;
    ss << "com.sina.weibo.stage" << " " << "1" << " " << "1" << std::endl;
    ss << "com.xxx.test.test1" << " " << "1" << " " << "0" << std::endl;
    ss << "com.xxx.test.test2" << " " << "abc" << " " << "0" << std::endl;
    content = ss.str();
    CommonUtils::WriteFile(POLICY_DATABASE_BACKUP_FILE_TEST, content);
    
    UniqueFd fd = UniqueFd(open(POLICY_DATABASE_BACKUP_FILE_TEST, O_RDONLY));
    lseek(fd.Get(), 0, SEEK_SET);
    auto netpolicyClonePtr = std::make_shared<NetPolicyDBClone>();
    ASSERT_NE(netpolicyClonePtr, nullptr);
    netpolicyClonePtr->OnRestore(fd, "");
    close(fd.Release());
    EXPECT_NE(netpolicyClonePtr, nullptr);
}

HWTEST_F(NetPolicyDBCloneTest, FdCloneTest001, TestSize.Level1)
{
    auto netpolicyClonePtr = std::make_shared<NetPolicyDBClone>();
    ASSERT_NE(netpolicyClonePtr, nullptr);
    UniqueFd fdErr1;
    bool ret = netpolicyClonePtr->FdClone(fdErr1);
    EXPECT_EQ(ret, false);
    UniqueFd fdErr4(-1);
    ret = netpolicyClonePtr->FdClone(fdErr4);
    EXPECT_EQ(ret, false);
    UniqueFd fdErr5(0);
    ret = netpolicyClonePtr->FdClone(fdErr5);
    EXPECT_EQ(ret, false);
    UniqueFd fdErr2(111);
    ret = netpolicyClonePtr->FdClone(fdErr2);
    EXPECT_EQ(ret, false);
    UniqueFd fd = UniqueFd(open(POLICY_DATABASE_BACKUP_FILE_TEST, O_RDONLY));
    ret = netpolicyClonePtr->FdClone(fd);
    EXPECT_EQ(ret, true);
}

HWTEST_F(NetPolicyDBCloneTest, OnRestoreSingleAppTest001, TestSize.Level1)
{
    auto netpolicyClonePtr = std::make_shared<NetPolicyDBClone>();
    ASSERT_NE(netpolicyClonePtr, nullptr);
    std::string bundleNameFromListen = "";
    int ret = netpolicyClonePtr->OnRestoreSingleApp(bundleNameFromListen);
    EXPECT_EQ(ret, -1);

    NetAccessPolicyData policyData;
    netpolicyClonePtr->unInstallApps_["test"] = policyData;
    bundleNameFromListen = "test";
    int ret2 = netpolicyClonePtr->OnRestoreSingleApp(bundleNameFromListen);
    EXPECT_EQ(ret2, -1);

    NetAccessPolicyData policyData2;
    netpolicyClonePtr->unInstallApps_["com.ohos.sceneboard"] = policyData2;
    bundleNameFromListen = "com.ohos.sceneboard";
    int ret3 = netpolicyClonePtr->OnRestoreSingleApp(bundleNameFromListen);
    EXPECT_LE(ret3, 0);
}

HWTEST_F(NetPolicyDBCloneTest, ClearBackupInfoTest001, TestSize.Level1)
{
    auto netpolicyClonePtr = std::make_shared<NetPolicyDBClone>();
    ASSERT_NE(netpolicyClonePtr, nullptr);

    netpolicyClonePtr->ClearBackupInfo();
    EXPECT_NE(netpolicyClonePtr, nullptr);
}
}
}
