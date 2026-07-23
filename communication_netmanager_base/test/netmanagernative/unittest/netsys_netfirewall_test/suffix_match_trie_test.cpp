/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "suffix_match_trie.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS::NetManagerStandard;

namespace {
static const string TEST_SUFFIX1 = "harmony.com";
static const string TEST_SUFFIX2 = "openharmony.com";
static const string TEST_DOMAIN_STR1 = "mony.com";
static const string TEST_DOMAIN_STR2 = "test.harmony.com";
static const string TEST_DOMAIN_STR3 = "test.openharmony.com";
static const string TEST_DOMAIN_STR4 = "test.openharMONY.Com";
}

class SuffixMatchTrieTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SuffixMatchTrieTest::SetUpTestCase() {}

void SuffixMatchTrieTest::TearDownTestCase() {}

void SuffixMatchTrieTest::SetUp() {}

void SuffixMatchTrieTest::TearDown() {}

HWTEST_F(SuffixMatchTrieTest, SuffixMatchTrieTest001, TestSize.Level0)
{
    SuffixMatchTrie<int> trie;
    EXPECT_TRUE(trie.Empty());
    int val1 = 101;
    trie.Insert(TEST_SUFFIX1, val1);

    EXPECT_FALSE(trie.Empty());

    int val = 0;
    int len = trie.LongestSuffixMatch(TEST_DOMAIN_STR2, val);

    EXPECT_EQ(len, strlen(TEST_SUFFIX1.c_str()));
    EXPECT_EQ(val, 101);
}

HWTEST_F(SuffixMatchTrieTest, SuffixMatchTrieTest002, TestSize.Level0)
{
    SuffixMatchTrie<int> trie;
    int val1 = 101;
    trie.Insert(TEST_SUFFIX1, val1);
    int val = 0;
    int len = trie.LongestSuffixMatch(TEST_DOMAIN_STR1, val);

    EXPECT_EQ(len, 0);
    EXPECT_EQ(val, 0);
}

HWTEST_F(SuffixMatchTrieTest, SuffixMatchTrieTest003, TestSize.Level0)
{
    SuffixMatchTrie<int> trie;
    int val1 = 101;
    int val2 = 102;
    trie.Insert(TEST_SUFFIX1, val1);
    trie.Insert(TEST_SUFFIX2, val2);
    int val = 0;
    int len = trie.LongestSuffixMatch(TEST_DOMAIN_STR3, val);

    EXPECT_EQ(len, strlen(TEST_SUFFIX2.c_str()));
    EXPECT_EQ(val, 102);

    val = 0;
    len = 0;
    len = trie.LongestSuffixMatch(TEST_DOMAIN_STR4, val);

    EXPECT_EQ(len, strlen(TEST_SUFFIX2.c_str()));
    EXPECT_EQ(val, 102);
}

HWTEST_F(SuffixMatchTrieTest, SuffixMatchTrieTest004, TestSize.Level0)
{
    SuffixMatchTrie<int> trie;
    int val1 = 101;
    int val2 = 102;
    int val3 = 201;
    trie.Insert(TEST_SUFFIX1, val1);
    trie.Insert(TEST_SUFFIX2, val2);
    trie.Update(TEST_SUFFIX1, val3);

    int val = 0;
    int len = trie.LongestSuffixMatch(TEST_DOMAIN_STR2, val);

    EXPECT_EQ(len, strlen(TEST_SUFFIX1.c_str()));
    EXPECT_EQ(val, 201);
}