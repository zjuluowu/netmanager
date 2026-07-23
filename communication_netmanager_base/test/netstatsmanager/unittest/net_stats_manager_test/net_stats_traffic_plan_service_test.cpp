/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include "net_mgr_log_wrapper.h"
#include "net_stats_traffic_plan_service.h"
#include "net_manager_constants.h"
#include "mock_core_service_manager.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing;
using namespace testing::ext;

class NetStatsTrafficPlanServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

class MockCoreServiceClient {
public:
    MOCK_METHOD(int32_t, GetSlotId, (int32_t), (const));
};

void NetStatsTrafficPlanServiceTest::SetUpTestCase()
{
    NETMGR_LOG_D("NetStatsTrafficPlanServiceTest SetUpTestCase");
}

void NetStatsTrafficPlanServiceTest::TearDownTestCase()
{
    NETMGR_LOG_D("NetStatsTrafficPlanServiceTest TearDownTestCase");
}

void NetStatsTrafficPlanServiceTest::SetUp()
{
    NETMGR_LOG_D("NetStatsTrafficPlanServiceTest SetUp");
}

void NetStatsTrafficPlanServiceTest::TearDown()
{
    NETMGR_LOG_D("NetStatsTrafficPlanServiceTest TearDown");
}
#ifdef SUPPORT_TRAFFIC_STATISTIC
/**
 * @tc.number: NetStatsTrafficPlanService_InitTrafficPlanInfo_Normal
 * @tc.name: InitTrafficPlanInfo normal path
 * @tc.desc: Test normal initialization of traffic plan info from database
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, InitTrafficPlanInfo_Normal, TestSize.Level1)
{
    // Arrange - Stub database query to return success
    int32_t simId = 1;
    std::u16string iccid = {u"1234564654651"};
    // MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSimIccId(_, _)).
        WillRepeatedly(DoAll(SetArgReferee<1>(iccid), Return(0)));
    NetStatsTrafficPlanService trafficPlanService;
    // Act
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Assert - Traffic plan info should be initialized
    EXPECT_TRUE(trafficPlanService.IsSimIdExistInMap(simId));
}

/**
 * @tc.number: NetStatsTrafficPlanService_InitTrafficPlanInfo_SimIdExists
 * @tc.name: InitTrafficPlanInfo with existing simId
 * @tc.desc: Test that initialization skips when simId already exists in map
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, InitTrafficPlanInfo_SimIdExists, TestSize.Level1)
{
    // Arrange - Initialize once to create entry
    int32_t simId = 1;
    // MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));

    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act - Initialize again with same simId
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Assert - Should not cause any error, just return early
    EXPECT_TRUE(trafficPlanService.IsSimIdExistInMap(simId));
}

/**
 * @tc.number: NetStatsTrafficPlanService_DeleteTrafficPlanInfo_Normal
 * @tc.name: DeleteTrafficPlanInfo normal path
 * @tc.desc: Test normal deletion of traffic plan info
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, DeleteTrafficPlanInfo_Normal, TestSize.Level1)
{
    // Arrange - Add traffic plan info
    int32_t simId = 1;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));

    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act - Delete traffic plan info
    int32_t slotId = 0;
    trafficPlanService.DeleteTrafficPlanInfo(slotId);

    // Assert - Traffic plan info should be removed
    // Note: Need to verify deletion logic
    EXPECT_FALSE(trafficPlanService.IsSimIdExistInMap(simId));
}

/**
 * @tc.number: NetStatsTrafficPlanService_DeleteTrafficPlanInfo_NotFound
 * @tc.name: DeleteTrafficPlanInfo with non-existent slot
 * @tc.desc: Test deletion when slotId is not found in map
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, DeleteTrafficPlanInfo_NotFound, TestSize.Level1)
{
    // Arrange - Non-existent slotId
    int32_t slotId = 999;

    // Act - Delete with non-existent slotId
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.DeleteTrafficPlanInfo(slotId);

    EXPECT_TRUE(trafficPlanService.trafficPlanInfoMap_.find(slotId) == trafficPlanService.trafficPlanInfoMap_.end());
}

/**
 * @tc.number: NetStatsTrafficPlanService_DeleteTrafficPlanInfo_EmptyMap
 * @tc.name: DeleteTrafficPlanInfo with empty map
 * @tc.desc: Test deletion when traffic plan info map is empty
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, DeleteTrafficPlanInfo_EmptyMap, TestSize.Level1)
{
    // Arrange - Empty map (no initialization)

    // Act - Delete from empty map
    int32_t slotId = 0;
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.DeleteTrafficPlanInfo(slotId);

    EXPECT_TRUE(trafficPlanService.trafficPlanInfoMap_.find(slotId) == trafficPlanService.trafficPlanInfoMap_.end());
}

/**
 * @tc.number: NetStatsTrafficPlanService_SetTrafficPlanInfo_Normal
 * @tc.name: SetTrafficPlanInfo normal path
 * @tc.desc: Test normal setting of traffic plan info
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, SetTrafficPlanInfo_Normal, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    TrafficPlanParam param = TrafficPlanParam::UNLIMIT_TRAFFIC_SWITCH;
    int64_t value = 1;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));

    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    int32_t ret = trafficPlanService.SetTrafficPlanInfo(simId, param, value);

    // Assert
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: NetStatsTrafficPlanService_SetTrafficPlanInfo_InvalidSimId
 * @tc.name: SetTrafficPlanInfo with invalid simId
 * @tc.desc: Test error handling when simId is invalid
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, SetTrafficPlanInfo_InvalidSimId, TestSize.Level1)
{
    // Arrange
    int32_t simId = -1;
    TrafficPlanParam param = TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH;
    int64_t value = 1;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(-1));

    // Act
    NetStatsTrafficPlanService trafficPlanService;
    int32_t ret = trafficPlanService.SetTrafficPlanInfo(simId, param, value);

    // Assert
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

/**
 * @tc.number: NetStatsTrafficPlanService_SetTrafficPlanInfo_InvalidParam
 * @tc.name: SetTrafficPlanInfo with invalid param
 * @tc.desc: Test error handling when param value is invalid
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, SetTrafficPlanInfo_InvalidParam, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    TrafficPlanParam param = static_cast<TrafficPlanParam>(99);
    int64_t value = 1;

    // Act
    NetStatsTrafficPlanService trafficPlanService;
    int32_t ret = trafficPlanService.SetTrafficPlanInfo(simId, param, value);

    // Assert
    EXPECT_EQ(ret, TRAFFIC_PLAN_ERR_INVALID_PARAM);
}

/**
 * @tc.number: NetStatsTrafficPlanService_SetTrafficPlanInfo_InvalidValue
 * @tc.name: SetTrafficPlanInfo with invalid value
 * @tc.desc: Test error handling when value is invalid for the param type
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, SetTrafficPlanInfo_InvalidValue, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    TrafficPlanParam param = TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH;
    int64_t value = 2; // Invalid: should be 0 or 1

    // Act
    NetStatsTrafficPlanService trafficPlanService;
    int32_t ret = trafficPlanService.SetTrafficPlanInfo(simId, param, value);

    // Assert
    EXPECT_EQ(ret, TRAFFIC_PLAN_ERR_INVALID_PARAM);
}

/**
 * @tc.number: NetStatsTrafficPlanService_SetTrafficPlanInfo_DisplaySwitch
 * @tc.name: SetTrafficPlanInfo display switch param
 * @tc.desc: Test setting display traffic switch parameter
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, SetTrafficPlanInfo_DisplaySwitch, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    TrafficPlanParam param = TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH;
    int64_t value = 1;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    int32_t ret = trafficPlanService.SetTrafficPlanInfo(simId, param, value);

    // Assert
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: NetStatsTrafficPlanService_SetTrafficPlanInfo_TrafficLimit
 * @tc.name: SetTrafficPlanInfo traffic limit param
 * @tc.desc: Test setting traffic limit parameter
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, SetTrafficPlanInfo_TrafficLimit, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    TrafficPlanParam param = TrafficPlanParam::TRAFFIC_LIMIT;
    int64_t value = 1024 * 1024 * 1024; // 1GB
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    int32_t ret = trafficPlanService.SetTrafficPlanInfo(simId, param, value);

    // Assert
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: NetStatsTrafficPlanService_GetTrafficPlanInfo_CacheHit
 * @tc.name: GetTrafficPlanInfo cache hit
 * @tc.desc: Test getting traffic plan info from cache
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, GetTrafficPlanInfo_CacheHit, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    TrafficPlanParam param = TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH;
    int64_t value = 1;
    int64_t result = 0;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));

    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);
    trafficPlanService.SetTrafficPlanInfo(simId, param, value);

    // Act
    int32_t ret = trafficPlanService.GetTrafficPlanInfo(simId, param, result);

    // Assert
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(result, value);
}

/**
 * @tc.number: NetStatsTrafficPlanService_GetTrafficPlanInfo_InvalidSimId
 * @tc.name: GetTrafficPlanInfo with invalid simId
 * @tc.desc: Test error handling when simId is invalid
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, GetTrafficPlanInfo_InvalidSimId, TestSize.Level1)
{
    // Arrange
    int32_t simId = -1;
    TrafficPlanParam param = TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(-1));
    int64_t result = 0;

    // Act
    NetStatsTrafficPlanService trafficPlanService;
    int32_t ret = trafficPlanService.GetTrafficPlanInfo(simId, param, result);

    // Assert
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

/**
 * @tc.number: NetStatsTrafficPlanService_ValidateTrafficPlanParam_DisplaySwitch_Valid
 * @tc.name: ValidateTrafficPlanParam with valid display switch
 * @tc.desc: Test validation of display traffic switch with valid value
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, ValidateTrafficPlanParam_DisplaySwitch_Valid, TestSize.Level1)
{
    // Arrange
    TrafficPlanParam param = TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH;
    int64_t value = 1;

    // Act - This is a private method, need to access via public interface or test mode
    // For now, we'll test indirectly through SetTrafficPlanInfo
    int32_t simId = 1;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    int32_t ret = trafficPlanService.SetTrafficPlanInfo(simId, param, value);

    // Assert
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: NetStatsTrafficPlanService_ValidateTrafficPlanParam_DisplaySwitch_Invalid
 * @tc.name: ValidateTrafficPlanParam with invalid display switch
 * @tc.desc: Test validation of display traffic switch with invalid value
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, ValidateTrafficPlanParam_DisplaySwitch_Invalid, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    TrafficPlanParam param = TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH;
    int64_t value = 2; // Invalid: should be 0 or 1
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    int32_t ret = trafficPlanService.SetTrafficPlanInfo(simId, param, value);

    // Assert
    EXPECT_EQ(ret, TRAFFIC_PLAN_ERR_INVALID_PARAM);
}

/**
 * @tc.number: NetStatsTrafficPlanService_ValidateTrafficPlanParam_TrafficLimit_Valid
 * @tc.name: ValidateTrafficPlanParam with valid traffic limit
 * @tc.desc: Test validation of traffic limit with valid value
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, ValidateTrafficPlanParam_TrafficLimit_Valid, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    TrafficPlanParam param = TrafficPlanParam::TRAFFIC_LIMIT;
    int64_t value = 1024 * 1024 * 1024; // 1GB
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));

    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    int32_t ret = trafficPlanService.SetTrafficPlanInfo(simId, param, value);

    // Assert
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsTrafficPlanServiceTest, ValidateTrafficPlanParam_TrafficLimit_valid2, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    TrafficPlanParam param = TrafficPlanParam::TRAFFIC_LIMIT;
    int64_t value = -1;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));

    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    int32_t ret = trafficPlanService.SetTrafficPlanInfo(simId, param, value);

    // Assert
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: NetStatsTrafficPlanService_ValidateTrafficPlanParam_TrafficLimit_Invalid
 * @tc.name: ValidateTrafficPlanParam with invalid traffic limit
 * @tc.desc: Test validation of traffic limit with invalid value
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, ValidateTrafficPlanParam_TrafficLimit_Invalid, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    TrafficPlanParam param = TrafficPlanParam::TRAFFIC_LIMIT;
    int64_t value = -2; // Invalid: should be -1 or >= 0
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));

    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    int32_t ret = trafficPlanService.SetTrafficPlanInfo(simId, param, value);

    // Assert
    EXPECT_EQ(ret, TRAFFIC_PLAN_ERR_INVALID_PARAM);
}

/**
 * @tc.number: NetStatsTrafficPlanService_GetFieldValueByParam_AllTypes
 * @tc.name: GetFieldValueByParam all param types
 * @tc.desc: Test getting field values for all parameter types
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, GetFieldValueByParam_AllTypes, TestSize.Level1)
{
    // Arrange - Test via GetTrafficPlanInfo
    int32_t simId = 1;
    int64_t result = 0;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));

    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act & Assert - Test each parameter type
    TrafficPlanParam params[] = {TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH, TrafficPlanParam::UNLIMIT_TRAFFIC_SWITCH,
                                 TrafficPlanParam::TRAFFIC_LIMIT,          TrafficPlanParam::START_DATE,
                                 TrafficPlanParam::OVER_LIMIT_BEHAVIOR,    TrafficPlanParam::MONTHLY_LIMIT_PERCENTAGE,
                                 TrafficPlanParam::DAILY_LIMIT_PERCENTAGE};

    for (const auto &param : params) {
        result = 0;
        int32_t ret = trafficPlanService.GetTrafficPlanInfo(simId, param, result);
        EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    }
}

/**
 * @tc.number: NetStatsTrafficPlanService_OnBackup_Normal
 * @tc.name: OnBackup normal path
 * @tc.desc: Test normal backup operation
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, OnBackup_Normal, TestSize.Level1)
{
    // Arrange
    MessageParcel data;
    MessageParcel reply;

    // Act
    NetStatsTrafficPlanService trafficPlanService;
    int32_t ret = trafficPlanService.OnBackup(data, reply);

    // Assert
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number: NetStatsTrafficPlanService_OnRestore_Normal
 * @tc.name: OnRestore normal path
 * @tc.desc: Test normal restore operation
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, OnRestore_Normal, TestSize.Level1)
{
    // Arrange
    MessageParcel data;
    MessageParcel reply;

    // Act
    NetStatsTrafficPlanService trafficPlanService;
    int32_t ret = trafficPlanService.OnRestore(data, reply);

    // Assert
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.number: NetStatsTrafficPlanService_ResetNotifyState_Normal
 * @tc.name: ResetNotifyState normal path
 * @tc.desc: Test normal reset of notification state
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, ResetNotifyState_Normal, TestSize.Level1)
{
    // Arrange - Initialize traffic plan info
    int32_t simId = 1;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    trafficPlanService.ResetNotifyState(simId);

    EXPECT_TRUE(trafficPlanService.trafficPlanInfoMap_.find(simId) != trafficPlanService.trafficPlanInfoMap_.end());
}

/**
 * @tc.number: NetStatsTrafficPlanService_ResetNotifyState_SimIdNotFound
 * @tc.name: ResetNotifyState with non-existent simId
 * @tc.desc: Test reset when simId is not found in map
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, ResetNotifyState_SimIdNotFound, TestSize.Level1)
{
    // Arrange
    int32_t simId = 999;

    // Act
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.ResetNotifyState(simId);

    EXPECT_FALSE(trafficPlanService.trafficPlanInfoMap_.find(simId) != trafficPlanService.trafficPlanInfoMap_.end());
}

/**
 * @tc.number: NetStatsTrafficPlanService_UpdateTrafficLimitDate_Normal
 * @tc.name: UpdateTrafficLimitDate normal path
 * @tc.desc: Test normal update of traffic limit date
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, UpdateTrafficLimitDate_Normal, TestSize.Level1)
{
    // Arrange - Initialize traffic plan info
    int32_t simId = 1;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    trafficPlanService.UpdateTrafficLimitDate(simId);

    EXPECT_TRUE(trafficPlanService.trafficPlanInfoMap_.find(simId) != trafficPlanService.trafficPlanInfoMap_.end());
}

/**
 * @tc.number: NetStatsTrafficPlanService_IsSimIdExistInMap_True
 * @tc.name: IsSimIdExistInMap when simId exists
 * @tc.desc: Test checking existence when simId is in map
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, IsSimIdExistInMap_True, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    bool exists = trafficPlanService.IsSimIdExistInMap(simId);

    // Assert
    EXPECT_TRUE(exists);
}

/**
 * @tc.number: NetStatsTrafficPlanService_IsSimIdExistInMap_False
 * @tc.name: IsSimIdExistInMap when simId not found
 * @tc.desc: Test checking existence when simId is not in map
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, IsSimIdExistInMap_False, TestSize.Level1)
{
    // Arrange
    int32_t simId = 999;

    // Act
    NetStatsTrafficPlanService trafficPlanService;
    bool exists = trafficPlanService.IsSimIdExistInMap(simId);

    // Assert
    EXPECT_FALSE(exists);
}

/**
 * @tc.number: NetStatsTrafficPlanService_GetTrafficPlanInfoBySimId_Found
 * @tc.name: GetTrafficPlanInfoBySimId when found
 * @tc.desc: Test getting traffic plan info when simId exists
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, GetTrafficPlanInfoBySimId_Found, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    auto info = trafficPlanService.GetTrafficPlanInfoBySimId(simId);

    // Assert
    EXPECT_NE(info, nullptr);
}

/**
 * @tc.number: NetStatsTrafficPlanService_GetTrafficPlanInfoBySimId_NotFound
 * @tc.name: GetTrafficPlanInfoBySimId when not found
 * @tc.desc: Test getting traffic plan info when simId doesn't exist
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, GetTrafficPlanInfoBySimId_NotFound, TestSize.Level1)
{
    // Arrange
    int32_t simId = 999;

    // Act
    NetStatsTrafficPlanService trafficPlanService;
    auto info = trafficPlanService.GetTrafficPlanInfoBySimId(simId);

    // Assert
    EXPECT_EQ(info, nullptr);
}

/**
 * @tc.number: NetStatsTrafficPlanService_GetMonthlyLimitBySimId_Found
 * @tc.name: GetMonthlyLimitBySimId when found
 * @tc.desc: Test getting monthly limit when simId exists
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, GetMonthlyLimitBySimId_Found, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    uint64_t monthlyLimit = 0;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    bool ret = trafficPlanService.GetMonthlyLimitBySimId(simId, monthlyLimit);

    // Assert
    EXPECT_TRUE(ret);
}

/**
 * @tc.number: NetStatsTrafficPlanService_GetMonthlyLimitBySimId_NotFound
 * @tc.name: GetMonthlyLimitBySimId when not found
 * @tc.desc: Test getting monthly limit when simId doesn't exist
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, GetMonthlyLimitBySimId_NotFound, TestSize.Level1)
{
    // Arrange
    int32_t simId = 999;
    uint64_t monthlyLimit = 0;

    // Act
    NetStatsTrafficPlanService trafficPlanService;
    bool ret = trafficPlanService.GetMonthlyLimitBySimId(simId, monthlyLimit);

    // Assert
    EXPECT_FALSE(ret);
}

/**
 * @tc.number: NetStatsTrafficPlanService_GetMonthlyMarkBySimId_Found
 * @tc.name: GetMonthlyMarkBySimId when found
 * @tc.desc: Test getting monthly percentage mark when simId exists
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, GetMonthlyMarkBySimId_Found, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    uint16_t monthlyMark = 0;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    bool ret = trafficPlanService.GetMonthlyMarkBySimId(simId, monthlyMark);

    // Assert
    EXPECT_TRUE(ret);
}

/**
 * @tc.number: NetStatsTrafficPlanService_GetMonthlyMarkBySimId_NotFound
 * @tc.name: GetMonthlyMarkBySimId when not found
 * @tc.desc: Test getting monthly percentage mark when simId doesn't exist
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, GetMonthlyMarkBySimId_NotFound, TestSize.Level1)
{
    // Arrange
    int32_t simId = 999;
    uint16_t monthlyMark = 0;

    // Act
    NetStatsTrafficPlanService trafficPlanService;
    bool ret = trafficPlanService.GetMonthlyMarkBySimId(simId, monthlyMark);

    // Assert
    EXPECT_FALSE(ret);
}

/**
 * @tc.number: NetStatsTrafficPlanService_GetdailyMarkBySimId_Found
 * @tc.name: GetDailyMarkBySimId when found
 * @tc.desc: Test getting daily percentage mark when simId exists
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, GetdailyMarkBySimId_Found, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    uint16_t dailyMark = 0;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    bool ret = trafficPlanService.GetDailyMarkBySimId(simId, dailyMark);

    // Assert
    EXPECT_TRUE(ret);
}

/**
 * @tc.number: NetStatsTrafficPlanService_GetdailyMarkBySimId_NotFound
 * @tc.name: GetDailyMarkBySimId when not found
 * @tc.desc: Test getting daily percentage mark when simId doesn't exist
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, GetdailyMarkBySimId_NotFound, TestSize.Level1)
{
    // Arrange
    int32_t simId = 999;
    uint16_t dailyMark = 0;

    // Act
    NetStatsTrafficPlanService trafficPlanService;
    bool ret = trafficPlanService.GetDailyMarkBySimId(simId, dailyMark);

    // Assert
    EXPECT_FALSE(ret);
}

/**
 * @tc.number: NetStatsTrafficPlanService_UpdateNetStatsToMapFromDB_Normal
 * @tc.name: UpdateNetStatsToMapFromDB normal path
 * @tc.desc: Test normal update of net stats from database
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, UpdateNetStatsToMapFromDB_Normal, TestSize.Level1)
{
    // Arrange
    int32_t simId = 1;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    trafficPlanService.UpdateNetStatsToMapFromDB(simId);

    EXPECT_TRUE(trafficPlanService.trafficPlanInfoMap_.find(simId) != trafficPlanService.trafficPlanInfoMap_.end());
}

/**
 * @tc.number: NetStatsTrafficPlanService_UpdateNetStatsToMapFromDB_EmptyResult
 * @tc.name: UpdateNetStatsToMapFromDB with empty DB result
 * @tc.desc: Test update when database returns empty result
 */
HWTEST_F(NetStatsTrafficPlanServiceTest, UpdateNetStatsToMapFromDB_EmptyResult, TestSize.Level1)
{
    // Arrange - Initialize without DB data
    int32_t simId = 2;
    MockCoreServiceClient mockClient;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(simId)).WillRepeatedly(Return(0));
    NetStatsTrafficPlanService trafficPlanService;
    trafficPlanService.InitTrafficPlanInfo(simId);

    // Act
    trafficPlanService.UpdateNetStatsToMapFromDB(simId);

    // Assert - Should handle empty result gracefully
    EXPECT_TRUE(trafficPlanService.trafficPlanInfoMap_.find(simId) != trafficPlanService.trafficPlanInfoMap_.end());
}
#endif
} // namespace NetManagerStandard
} // namespace OHOS
