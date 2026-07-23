/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <ctime>
#include <thread>
#include <vector>

#include <gtest/gtest.h>
#ifndef USE_SQLITE_SYMBOLS
#include "sqlite3.h"
#else
#include "sqlite3sym.h"
#endif

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_stats_sqlite_statement.h"
#include "net_stats_database_helper.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
#define DTEST_LOG std::cout << __func__ << ":" << __LINE__ << ": "
constexpr int32_t SQLITE_ERRORNUM = 21;
} // namespace


class NetStatsSqliteStatementTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetStatsSqliteStatement> instance_ = nullptr;
};

void NetStatsSqliteStatementTest::SetUpTestCase()
{
    instance_ = std::make_shared<NetStatsSqliteStatement>();
}

void NetStatsSqliteStatementTest::TearDownTestCase()
{
}

void NetStatsSqliteStatementTest::SetUp() {}

void NetStatsSqliteStatementTest::TearDown() {}

HWTEST_F(NetStatsSqliteStatementTest, PrepareTest001, TestSize.Level1)
{
    const std::string testSql = "test sql";
    auto result = instance_->Prepare(nullptr, testSql);
    DTEST_LOG << "Prepare result: " << result << std::endl;
    EXPECT_NE(result, SQLITE_OK);
}

HWTEST_F(NetStatsSqliteStatementTest, FinalizeTest001, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    instance_->Finalize();
    EXPECT_NE(instance_, nullptr);
}

HWTEST_F(NetStatsSqliteStatementTest, FinalizeTest002, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    instance_->Finalize();
    EXPECT_NE(instance_, nullptr);
}

HWTEST_F(NetStatsSqliteStatementTest, ResetStatementAndClearBindingsTest001, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    instance_->ResetStatementAndClearBindings();
    EXPECT_NE(instance_, nullptr);
}

HWTEST_F(NetStatsSqliteStatementTest, GetColumnStringTest001, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    std::string emptyValue;
    auto result = instance_->GetColumnString(-1, emptyValue);
    EXPECT_EQ(result, SQLITE_ERROR);
}

HWTEST_F(NetStatsSqliteStatementTest, GetColumnStringTest002, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    std::string emptyValue;
    instance_->columnCount_ = 100;
    auto result = instance_->GetColumnString(instance_->columnCount_ + 1, emptyValue);
    EXPECT_EQ(result, SQLITE_ERROR);
}

HWTEST_F(NetStatsSqliteStatementTest, GetColumnStringTest003, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    std::string emptyValue;
    instance_->columnCount_ = 100;
    auto result = instance_->GetColumnString(instance_->columnCount_ - 1, emptyValue);
    EXPECT_EQ(result, SQLITE_ERROR);
}

HWTEST_F(NetStatsSqliteStatementTest, GetColumnLongTest001, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    uint64_t emptyValue;
    auto result = instance_->GetColumnLong(-1, emptyValue);
    EXPECT_EQ(result, SQLITE_ERROR);
}

HWTEST_F(NetStatsSqliteStatementTest, GetColumnLongTest002, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    uint64_t emptyValue;
    instance_->columnCount_ = 100;
    auto result = instance_->GetColumnLong(instance_->columnCount_ + 1, emptyValue);
    EXPECT_EQ(result, SQLITE_ERROR);
}

HWTEST_F(NetStatsSqliteStatementTest, GetColumnLongTest003, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    uint64_t emptyValue;
    instance_->columnCount_ = 100;
    auto result = instance_->GetColumnLong(instance_->columnCount_ - 1, emptyValue);
    EXPECT_EQ(result, SQLITE_ERROR);
}

HWTEST_F(NetStatsSqliteStatementTest, GetColumnIntTest001, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    uint32_t emptyValue;
    auto result = instance_->GetColumnInt(-1, emptyValue);
    EXPECT_EQ(result, SQLITE_ERROR);
}

HWTEST_F(NetStatsSqliteStatementTest, GetColumnIntTest002, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    uint32_t emptyValue;
    instance_->columnCount_ = 100;
    auto result = instance_->GetColumnInt(instance_->columnCount_ + 1, emptyValue);
    EXPECT_EQ(result, SQLITE_ERROR);
}

HWTEST_F(NetStatsSqliteStatementTest, GetColumnIntTest003, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    uint32_t emptyValue;
    instance_->columnCount_ = 100;
    auto result = instance_->GetColumnInt(instance_->columnCount_ - 1, emptyValue);
    EXPECT_EQ(result, SQLITE_ERROR);
}

HWTEST_F(NetStatsSqliteStatementTest, BindInt32001, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    int32_t emptyValue = 0;
    auto result = instance_->BindInt32(-1, emptyValue);
    EXPECT_EQ(result, SQLITE_ERRORNUM);
}

HWTEST_F(NetStatsSqliteStatementTest, BindInt64001, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    int64_t emptyValue = 0;
    instance_->columnCount_ = 100;
    auto result = instance_->BindInt64(-1, emptyValue);
    EXPECT_EQ(result, SQLITE_ERRORNUM);
}

HWTEST_F(NetStatsSqliteStatementTest, BindText001, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    std::string emptyValue;
    instance_->columnCount_ = 100;
    auto result = instance_->BindText(-1, emptyValue);
    EXPECT_EQ(result, SQLITE_ERRORNUM);
}

HWTEST_F(NetStatsSqliteStatementTest, Step001, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    instance_->columnCount_ = 100;
    auto result = instance_->Step();
    EXPECT_EQ(result, SQLITE_ERRORNUM);
}

HWTEST_F(NetStatsSqliteStatementTest, GetColumnCount001, TestSize.Level1)
{
    instance_->stmtHandle_ = nullptr;
    instance_->columnCount_ = 100;
    auto result = instance_->GetColumnCount();
    EXPECT_EQ(result, 100);
}
} // namespace NetManagerStandard
} // namespace OHOS