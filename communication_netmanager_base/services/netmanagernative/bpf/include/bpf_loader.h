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

#ifndef CONNECTIVITY_EXT_BPF_LOADER_H
#define CONNECTIVITY_EXT_BPF_LOADER_H

#include <string>

namespace OHOS::NetManagerStandard {
enum ElfLoadError {
    ELF_LOAD_ERR_NONE = 0,
    ELF_LOAD_ERR_PATH_INVALID,
    ELF_LOAD_ERR_MAKE_DIR_FAIL,
    ELF_LOAD_ERR_LOAD_FILE_FAIL,
    ELF_LOAD_ERR_GET_VERSION_FAIL,
    ELF_LOAD_ERR_SELECT_LICENSE_AND_VERSION_FAIL,
    ELF_LOAD_ERR_LOAD_MAP_SECTION_FAIL,
    ELF_LOAD_ERR_SET_RLIMIT_FAIL,
    ELF_LOAD_ERR_CREATE_MAP_FAIL,
    ELF_LOAD_ERR_PARSE_RELOCATION_FAIL,
    ELF_LOAD_ERR_LOAD_PROGS_FAIL,
    ELF_LOAD_ERR_DELETE_MAP_FAIL,
    ELF_LOAD_ERR_UNLOAD_PROGS_FAIL,
};

ElfLoadError LoadElf(const std::string &elfPath);

ElfLoadError UnloadElf(const std::string &elfPath);
} // namespace OHOS::NetManagerStandard
#endif /* CONNECTIVITY_EXT_BPF_LOADER_H */
