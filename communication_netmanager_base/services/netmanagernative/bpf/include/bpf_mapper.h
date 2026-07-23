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

#ifndef CONNECTIVITY_EXT_BPF_MAPPER_H
#define CONNECTIVITY_EXT_BPF_MAPPER_H

#include <cerrno>
#include <linux/bpf.h>
#include <linux/unistd.h>
#include <cstdint>
#include <fcntl.h>
#include <sys/stat.h>
#include <string>
#include <unistd.h>
#include <functional>
#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/unistd.h>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "securec.h"

namespace OHOS::NetManagerStandard {
template <class Key, class Value> class BpfMapperImplement {
public:
    BpfMapperImplement<Key, Value>() = default;

    static int32_t GetFirstKey(const int32_t mapFd, Key &key)
    {
        bpf_attr bpfAttr{};
        if (memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr)) != EOK) {
            return NETMANAGER_ERROR;
        }
        bpfAttr.map_fd = BpfFdToU32(mapFd);
        bpfAttr.key = 0;
        bpfAttr.next_key = BpfMapKeyToU64(key);
        return BpfSyscall(BPF_MAP_GET_NEXT_KEY, bpfAttr);
    }

    /**
     * Get the Next Key From Map
     *
     * @param mapFd map fd
     * @param key the key of Bpf Map
     * @param next_key the key of Bpf Map
     * @return int32_t return next key
     */
    static int32_t GetNextKey(const int32_t mapFd, const Key &key, Key &nextKey)
    {
        bpf_attr bpfAttr{};
        if (memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr)) != EOK) {
            return NETMANAGER_ERROR;
        }
        bpfAttr.map_fd = BpfFdToU32(mapFd);
        bpfAttr.key = BpfMapKeyToU64(key);
        bpfAttr.next_key = BpfMapKeyToU64(nextKey);
        return BpfSyscall(BPF_MAP_GET_NEXT_KEY, bpfAttr);
    }

    /**
     * Bpf Syscall
     *
     * @param cmd which command need to execute
     * @param attr union consists of various anonymous structures
     * @return int32_t return the result of executing the command
     */
    static int32_t BpfSyscall(int32_t cmd, const bpf_attr &attr)
    {
        return static_cast<int32_t>(syscall(__NR_bpf, cmd, &attr, sizeof(attr)));
    }

    static int32_t BpfSyscallAndLog(int32_t cmd, const bpf_attr &attr)
    {
        int32_t ret = static_cast<int32_t>(syscall(__NR_bpf, cmd, &attr, sizeof(attr)));
        // LCOV_EXCL_START
        if (ret < 0) {
            NETNATIVE_LOGE("syscall failed, ret:%{public}d, cmd:%{public}d, errno: %{public}u",
                ret, cmd, errno);
        }
        // LCOV_EXCL_STOP
        return ret;
    }

    /**
     * Write Value To Bpf Map
     *
     * @param mapFd map fd
     * @param key the key of Bpf Map
     * @param value the value of Bpf Map
     * @param flags map flag
     * @return int32_t 0:write success -1:failure
     */
    static int32_t UpdateElem(const int32_t mapFd, const Key &key, const Value &value, uint64_t flags)
    {
        bpf_attr bpfAttr{};
        if (memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr)) != EOK) {
            return NETMANAGER_ERROR;
        }
        bpfAttr.map_fd = BpfFdToU32(mapFd);
        bpfAttr.key = BpfMapKeyToU64(key);
        bpfAttr.value = BpfMapValueToU64(value);
        bpfAttr.flags = flags;
        return BpfSyscall(BPF_MAP_UPDATE_ELEM, bpfAttr);
    }

    /**
     * LookUp Elem From Map
     *
     * @param mapFd map fd
     * @param key the key of Bpf Map
     * @param value the value of Bpf Map
     * @return int32_t 0:find success -1:failure
     */
    static int32_t LookUpElem(const int32_t mapFd, const Key &key, const Value &value)
    {
        bpf_attr bpfAttr{};
        errno_t result = memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr));
        if (result != EOK) {
            NETNATIVE_LOGE("memset_s failed, result:%{public}d", result);
            return NETMANAGER_ERROR;
        }
        bpfAttr.map_fd = BpfFdToU32(mapFd);
        bpfAttr.key = BpfMapKeyToU64(key);
        bpfAttr.value = BpfMapValueToU64(value);
        return BpfSyscall(BPF_MAP_LOOKUP_ELEM, bpfAttr);
    }

    static int32_t LookUpElemAndLog(const int32_t mapFd, const Key &key, const Value &value)
    {
        bpf_attr bpfAttr{};
        errno_t result = memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr));
        // LCOV_EXCL_START
        if (result != EOK) {
            NETNATIVE_LOGE("memset_s failed, result:%{public}d", result);
            return NETMANAGER_ERROR;
        }
        // LCOV_EXCL_STOP
        bpfAttr.map_fd = BpfFdToU32(mapFd);
        bpfAttr.key = BpfMapKeyToU64(key);
        bpfAttr.value = BpfMapValueToU64(value);
        return BpfSyscallAndLog(BPF_MAP_LOOKUP_ELEM, bpfAttr);
    }

    /**
     * Delete Elem From Map
     *
     * @param mapFd map fd
     * @param key the key of Bpf Map
     * @return int32_t 0:delete success -1:failure
     */
    static int32_t DeleteElem(const int32_t mapFd, const Key &key)
    {
        bpf_attr bpfAttr{};
        if (memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr)) != EOK) {
            return NETMANAGER_ERROR;
        }
        bpfAttr.map_fd = BpfFdToU32(mapFd);
        bpfAttr.key = BpfMapKeyToU64(key);
        return BpfSyscall(BPF_MAP_DELETE_ELEM, bpfAttr);
    }

    /**
     * Get Bpf Object By PathName
     *
     * @param pathName bpf map path
     * @param fileFlags file flags
     * @return int32_t return map file descriptor
     */
    static int32_t BpfObjGet(const std::string &pathName, uint32_t fileFlags)
    {
        bpf_attr bpfAttr{};
        if (memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr)) != EOK) {
            return NETMANAGER_ERROR;
        }
        bpfAttr.pathname = BpfMapPathNameToU64(pathName);
        bpfAttr.file_flags = fileFlags;
        return BpfSyscall(BPF_OBJ_GET, bpfAttr);
    }

    /**
     * Get the Map Fd
     *
     * @param pathName bpf map path
     * @param objFlags obj flags
     * @return int32_t return map file descriptor
     */
    static int32_t GetMap(const std::string &pathName, uint32_t objFlags)
    {
        return BpfObjGet(pathName, objFlags);
    }

private:
    static uint32_t BpfFdToU32(const int32_t mapFd)
    {
        return static_cast<uint32_t>(mapFd);
    }

    static uint64_t BpfMapPathNameToU64(const std::string &pathName)
    {
        return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(pathName.c_str()));
    }

    static uint64_t BpfMapKeyToU64(const Key &key)
    {
        return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&key));
    }

    static uint64_t BpfMapValueToU64(const Value &value)
    {
        return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&value));
    }
};

template <class Key, class Value> class BpfMapper {
public:
    BpfMapper<Key, Value>() = default;
    ~BpfMapper<Key, Value>()
    {
        if (mapFd_ != NETMANAGER_ERROR) {
            close(mapFd_);
            mapFd_ = NETMANAGER_ERROR;
        }
    }
    BpfMapper<Key, Value>(const std::string &pathName, uint32_t flags)
    {
        mapFd_ = NETMANAGER_ERROR;
        int32_t mapFd = BpfMapperImplement<Key, Value>::GetMap(pathName, flags);
        if (mapFd >= 0) {
            mapFd_ = mapFd;
        }
    }

    /**
     * Is has map fd
     *
     * @return bool true:has map fd false:not have
     */
    [[nodiscard]] bool IsValid() const
    {
        return mapFd_ >= 0;
    }

    /**
     * Read Value From Map
     *
     * @param key the key of map
     * @return Value value corresponding to key
     */
    [[nodiscard]] int32_t Read(const Key &key, Value &val) const
    {
        Value value{};
        if (BpfMapperImplement<Key, Value>::LookUpElem(mapFd_, key, value) < 0) {
            return NETMANAGER_ERROR;
        }
        val = value;
        return 0;
    }

    [[nodiscard]] int32_t ReadAndLog(const Key &key, Value &val) const
    {
        Value value{};
        // LCOV_EXCL_START
        if (BpfMapperImplement<Key, Value>::LookUpElemAndLog(mapFd_, key, value) < 0) {
            return NETMANAGER_ERROR;
        }
        // LCOV_EXCL_STOP
        val = value;
        return 0;
    }

    /**
     * WriteValue
     *
     * @param key the key want to write
     * @param value the value want to write
     * @param flags map flag
     * @return int32_t 0 if OK
     */
    [[nodiscard]] int32_t Write(const Key &key, const Value &value, uint64_t flags) const
    {
        return BpfMapperImplement<Key, Value>::UpdateElem(mapFd_, key, value, flags);
    }

    /**
     * Get all keys
     *
     * @return key of list
     */
    [[nodiscard]] std::vector<Key> GetAllKeys() const
    {
        Key key{};
        if (BpfMapperImplement<Key, Value>::GetFirstKey(mapFd_, key) < 0) {
            return {};
        }
        std::vector<Key> keys;
        keys.emplace_back(key);

        Key nextKey{};
        while (BpfMapperImplement<Key, Value>::GetNextKey(mapFd_, key, nextKey) >= 0) {
            key = nextKey;
            keys.emplace_back(key);
        }
        return keys;
    }

    [[nodiscard]] int32_t Clear(const std::vector<Key> &keys) const
    {
        for (const auto &k : keys) {
            if (Delete(k) < NETSYS_SUCCESS) {
                return NETMANAGER_ERROR;
            }
        }
        return 0;
    }

    /**
     * DeleteEntryFromMap
     *
     * @param deleteKey the key need to delete
     * @return int32_t 0 if OK
     */
    [[nodiscard]] int32_t Delete(const Key &deleteKey) const
    {
        return BpfMapperImplement<Key, Value>::DeleteElem(mapFd_, deleteKey);
    }

private:
    int32_t mapFd_ = NETMANAGER_ERROR;
};
} // namespace OHOS::NetManagerStandard
#endif /* CONNECTIVITY_EXT_BPF_MAPPER_H */
