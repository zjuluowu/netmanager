/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

import { BusinessError } from '@ohos.base';
import type { AsyncCallback, Callback } from '@ohos.base';
import type connection from '@ohos.net.connection';
import Context from 'application.Context';

export default namespace policy {
  loadLibrary("policy_ani")

  export type NetBearType = connection.NetBearType;

  export enum NetBackgroundPolicy {
    NET_BACKGROUND_POLICY_NONE = 0,
    NET_BACKGROUND_POLICY_ENABLE = 1,
    NET_BACKGROUND_POLICY_DISABLE = 2,
    NET_BACKGROUND_POLICY_TRUSTLIST = 3,
  }

  export enum NetUidRule {
    NET_RULE_NONE = 0,
    NET_RULE_ALLOW_METERED_FOREGROUND = 1 << 0,
    NET_RULE_ALLOW_METERED = 1 << 1,
    NET_RULE_REJECT_METERED = 1 << 2,
    NET_RULE_ALLOW_ALL = 1 << 5,
    NET_RULE_REJECT_ALL = 1 << 6,
  }

  export enum RemindType {
    REMIND_TYPE_WARNING = 1,
    REMIND_TYPE_LIMIT = 2,
  }

  export enum NetUidPolicy {
    NET_POLICY_NONE = 0,
    NET_POLICY_ALLOW_METERED_BACKGROUND = 1 << 0,
    NET_POLICY_REJECT_METERED_BACKGROUND = 1 << 1,
  }

  export enum LimitAction {
    LIMIT_ACTION_NONE = -1,
    LIMIT_ACTION_ACCESS_DISABLED = 0,
    LIMIT_ACTION_ALERT_ONLY = 1,
  }

  export interface NetworkMatchRule {
    netType: int;
    identity: string;
    simId: string;
  }

  export interface QuotaPolicy {
    periodDuration: string;
    warningBytes: long;
    limitBytes: long;
    metered: boolean;
    limitAction: int;
    lastWarningRemind?: long;
    lastLimitRemind?: long;
  }

  export interface NetQuotaPolicy {
    networkMatchRule: NetworkMatchRule;
    quotaPolicy: QuotaPolicy;
  }

  export interface NetworkAccessPolicy {
    allowWiFi?: boolean;
    allowCellular?: boolean;
    alwaysAllowWiFi?: boolean;
    alwaysAllowCellular?: boolean;
  }

  export interface NetAccessPolicy {
    allowWiFi: boolean;
    allowCellular: boolean;
  }

  export interface UidRuleChangeInfo {
    uid: int;
    rule: int;
  }

  export interface UidPolicyChangeInfo {
    uid: int;
    policy: int;
  }

  export type UidNetworkAccessPolicy = Map<string, NetworkAccessPolicy>;

  export interface UidNetworkPolicyItem {
    uid: int;
    allowWiFi: boolean;
    allowCellular: boolean;
    alwaysAllowWiFi: boolean;
    alwaysAllowCellular: boolean;
  }

  export class NetAccessPolicyInner implements NetAccessPolicy {
    allowWiFi: boolean;
    allowCellular: boolean;
  }

  export native function getNetAccessPolicySync(): NetAccessPolicy;

  export function getNetAccessPolicy(): Promise<NetAccessPolicy> {
    return new Promise<NetAccessPolicy>((resolve, reject) => {
      taskpool.execute((): NetAccessPolicy => {
        return getNetAccessPolicySync();
      }).then((content: Any) => {
        resolve(content as NetAccessPolicy);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function onNetQuotaPolicyChangeSync(callback: Callback<Array<NetQuotaPolicy>>): void;
  export native function offNetQuotaPolicyChangeSync(callback?: Callback<Array<NetQuotaPolicy>>): void;

  export function onNetQuotaPolicyChange(callback: Callback<Array<NetQuotaPolicy>>): void {
    onNetQuotaPolicyChangeSync(callback);
  }

  export function offNetQuotaPolicyChange(callback?: Callback<Array<NetQuotaPolicy>>): void {
    offNetQuotaPolicyChangeSync(callback);
  }

  export native function onNetBackgroundPolicyChangeSync(callback: Callback<boolean>): void;
  export native function offNetBackgroundPolicyChangeSync(callback?: Callback<boolean>): void;

  export function onNetBackgroundPolicyChange(callback: Callback<boolean>): void {
    onNetBackgroundPolicyChangeSync(callback);
  }

  export function offNetBackgroundPolicyChange(callback?: Callback<boolean>): void {
    offNetBackgroundPolicyChangeSync(callback);
  }

  export native function onNetMeteredIfacesChangeSync(callback: Callback<Array<string>>): void;
  export native function offNetMeteredIfacesChangeSync(callback?: Callback<Array<string>>): void;

  export function onNetMeteredIfacesChange(callback: Callback<Array<string>>): void {
    onNetMeteredIfacesChangeSync(callback);
  }

  export function offNetMeteredIfacesChange(callback?: Callback<Array<string>>): void {
    offNetMeteredIfacesChangeSync(callback);
  }

  export native function onNetUidRuleChangeSync(callback: Callback<UidRuleChangeInfo>): void;
  export native function offNetUidRuleChangeSync(callback?: Callback<UidRuleChangeInfo>): void;

  export function onNetUidRuleChange(callback: Callback<UidRuleChangeInfo>): void {
    onNetUidRuleChangeSync(callback);
  }

  export function offNetUidRuleChange(callback?: Callback<UidRuleChangeInfo>): void {
    offNetUidRuleChangeSync(callback);
  }

  export native function onNetUidPolicyChangeSync(callback: Callback<UidPolicyChangeInfo>): void;
  export native function offNetUidPolicyChangeSync(callback?: Callback<UidPolicyChangeInfo>): void;

  export function onNetUidPolicyChange(callback: Callback<UidPolicyChangeInfo>): void {
    onNetUidPolicyChangeSync(callback);
  }

  export function offNetUidPolicyChange(callback?: Callback<UidPolicyChangeInfo>): void {
    offNetUidPolicyChangeSync(callback);
  }

  export native function showAppNetPolicySettingsSync(context: Context): void;

  export function showAppNetPolicySettings(context: Context): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        showAppNetPolicySettingsSync(context);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function setBackgroundAllowedSync(isAllowed: boolean): void;

  export function setBackgroundAllowed(isAllowed: boolean, callback: AsyncCallback<void>): void {
    taskpool.execute(() => {
      setBackgroundAllowedSync(isAllowed);
    }).then(() => callback(null, undefined),
      (err: Error): void => callback(err as BusinessError, undefined));
  }

  export function setBackgroundAllowed(isAllowed: boolean): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        setBackgroundAllowedSync(isAllowed);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function restoreAllPoliciesSync(iccid: string): void;

  export function restoreAllPolicies(iccid: string): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        restoreAllPoliciesSync(iccid);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function setPowerSaveTrustlistSync(uids: Array<int>, isAllowed: boolean): void;

  export function setPowerSaveTrustlist(uids: Array<int>, isAllowed: boolean, callback: AsyncCallback<void>): void {
    taskpool.execute(() => {
      setPowerSaveTrustlistSync(uids, isAllowed);
    }).then(() => callback(null, undefined),
      (err: Error): void => callback(err as BusinessError, undefined));
  }

  export function setPowerSaveTrustlist(uids: Array<int>, isAllowed: boolean): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        setPowerSaveTrustlistSync(uids, isAllowed);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function updateRemindPolicySync(netType: int, iccid: string, remindType: int): void;

  export function updateRemindPolicy(netType: int, iccid: string, remindType: int,
      callback: AsyncCallback<void>): void {
    taskpool.execute(() => {
      updateRemindPolicySync(netType, iccid, remindType);
    }).then(() => callback(null, undefined),
      (err: Error): void => callback(err as BusinessError, undefined));
  }

  export function updateRemindPolicy(netType: int, iccid: string, remindType: int): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        updateRemindPolicySync(netType, iccid, remindType);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function getPolicyByUidSync(uid: int): int;

  export function getPolicyByUid(uid: int, callback: AsyncCallback<int>): void {
    let p1 = taskpool.execute((): int => {
      return getPolicyByUidSync(uid);
    })
    p1.then((content: Any) => {
      callback(null, content as int);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  export function getPolicyByUid(uid: int): Promise<int> {
    return new Promise<int>((resolve, reject) => {
      taskpool.execute((): int => {
        return getPolicyByUidSync(uid);
      }).then((content: Any) => {
        resolve(content as int);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function isBackgroundAllowedSync(): boolean;

  export function isBackgroundAllowed(callback: AsyncCallback<boolean>): void {
    let p1 = taskpool.execute((): boolean => {
      return isBackgroundAllowedSync();
    })
    p1.then((content: Any) => {
      callback(null, content as boolean);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  export function isBackgroundAllowed(): Promise<boolean> {
    return new Promise<boolean>((resolve, reject) => {
      taskpool.execute((): boolean => {
        return isBackgroundAllowedSync();
      }).then((content: Any) => {
        resolve(content as boolean);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function resetPoliciesSync(iccid: string): void;

  export function resetPolicies(iccid: string, callback: AsyncCallback<void>): void {
    taskpool.execute(() => {
      resetPoliciesSync(iccid);
    }).then(() => callback(null, undefined),
      (err: Error): void => callback(err as BusinessError, undefined));
  }

  export function resetPolicies(iccid: string): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        resetPoliciesSync(iccid);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function getPowerSaveTrustlistSync(): Array<int>;

  export function getPowerSaveTrustlist(callback: AsyncCallback<Array<int>>): void {
    let p1 = taskpool.execute((): Array<int> => {
      return getPowerSaveTrustlistSync();
    })
    p1.then((content: Any) => {
      callback(null, content as Array<int>);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  export function getPowerSaveTrustlist(): Promise<Array<int>> {
    return new Promise<Array<int>>((resolve, reject) => {
      taskpool.execute((): Array<int> => {
        return getPowerSaveTrustlistSync();
      }).then((content: Any) => {
        resolve(content as Array<int>);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function getDeviceIdleTrustlistSync(): Array<int>;

  export function getDeviceIdleTrustlist(callback: AsyncCallback<Array<int>>): void {
    let p1 = taskpool.execute((): Array<int> => {
      return getDeviceIdleTrustlistSync();
    })
    p1.then((content: Any) => {
      callback(null, content as Array<int>);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  export function getDeviceIdleTrustlist(): Promise<Array<int>> {
    return new Promise<Array<int>>((resolve, reject) => {
      taskpool.execute((): Array<int> => {
        return getDeviceIdleTrustlistSync();
      }).then((content: Any) => {
        resolve(content as Array<int>);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function isUidNetAllowedSync(uid: int, isMetered: boolean): boolean;

  export function isUidNetAllowed(uid: int, isMetered: boolean, callback: AsyncCallback<boolean>): void {
    let p1 = taskpool.execute((): boolean => {
      return isUidNetAllowedSync(uid, isMetered);
    })
    p1.then((content: Any) => {
      callback(null, content as boolean);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  export function isUidNetAllowed(uid: int, isMetered: boolean): Promise<boolean> {
    return new Promise<boolean>((resolve, reject) => {
      taskpool.execute((): boolean => {
        return isUidNetAllowedSync(uid, isMetered);
      }).then((content: Any) => {
        resolve(content as boolean);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function isUidNetAllowedByIfaceSync(uid: int, iface: string): boolean;

  export function isUidNetAllowed(uid: int, iface: string, callback: AsyncCallback<boolean>): void {
    let p1 = taskpool.execute((): boolean => {
      return isUidNetAllowedByIfaceSync(uid, iface);
    })
    p1.then((content: Any) => {
      callback(null, content as boolean);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  export function isUidNetAllowed(uid: int, iface: string): Promise<boolean> {
    return new Promise<boolean>((resolve, reject) => {
      taskpool.execute((): boolean => {
        return isUidNetAllowedByIfaceSync(uid, iface);
      }).then((content: Any) => {
        resolve(content as boolean);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function setPolicyByUidSync(uid: int, policy: int): void;

  export function setPolicyByUid(uid: int, policy: int, callback: AsyncCallback<void>): void {
    taskpool.execute(() => {
      setPolicyByUidSync(uid, policy);
    }).then(() => callback(null, undefined),
      (err: Error): void => callback(err as BusinessError, undefined));
  }

  export function setPolicyByUid(uid: int, policy: int): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        setPolicyByUidSync(uid, policy);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function getBackgroundPolicyByUidSync(uid: int): int;

  export function getBackgroundPolicyByUid(uid: int, callback: AsyncCallback<int>): void {
    let p1 = taskpool.execute((): int => {
      return getBackgroundPolicyByUidSync(uid);
    })
    p1.then((content: Any) => {
      callback(null, content as int);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  export function getBackgroundPolicyByUid(uid: int): Promise<int> {
    return new Promise<int>((resolve, reject) => {
      taskpool.execute((): int => {
        return getBackgroundPolicyByUidSync(uid);
      }).then((content: Any) => {
        resolve(content as int);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function setDeviceIdleTrustlistSync(uids: Array<int>, isAllowed: boolean): void;

  export function setDeviceIdleTrustlist(uids: Array<int>, isAllowed: boolean, callback: AsyncCallback<void>): void {
    taskpool.execute(() => {
      setDeviceIdleTrustlistSync(uids, isAllowed);
    }).then(() => callback(null, undefined),
      (err: Error): void => callback(err as BusinessError, undefined));
  }

  export function setDeviceIdleTrustlist(uids: Array<int>, isAllowed: boolean): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        setDeviceIdleTrustlistSync(uids, isAllowed);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function getUidsByPolicySync(policy: int): Array<int>;

  export function getUidsByPolicy(policy: int, callback: AsyncCallback<Array<int>>): void {
    let p1 = taskpool.execute((): Array<int> => {
      return getUidsByPolicySync(policy);
    })
    p1.then((content: Any) => {
      callback(null, content as Array<int>);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  export function getUidsByPolicy(policy: int): Promise<Array<int>> {
    return new Promise<Array<int>>((resolve, reject) => {
      taskpool.execute((): Array<int> => {
        return getUidsByPolicySync(policy);
      }).then((content: Any) => {
        resolve(content as Array<int>);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function getNetworkAccessPolicyByUidSync(uid: int): NetworkAccessPolicy;

  export function getNetworkAccessPolicy(uid: int, callback: AsyncCallback<NetworkAccessPolicy>): void {
    let p1 = taskpool.execute((): NetworkAccessPolicy => {
      return getNetworkAccessPolicyByUidSync(uid);
    })
    p1.then((content: Any) => {
      callback(null, content as NetworkAccessPolicy);
    }, (err: Error): void => {
      callback(err as BusinessError, {} as NetworkAccessPolicy);
    });
  }

  export function getNetworkAccessPolicy(uid: int): Promise<NetworkAccessPolicy> {
    return new Promise<NetworkAccessPolicy>((resolve, reject) => {
      taskpool.execute((): NetworkAccessPolicy => {
        return getNetworkAccessPolicyByUidSync(uid);
      }).then((content: Any) => {
        resolve(content as NetworkAccessPolicy);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function getAllNetworkAccessPoliciesSync(): Array<UidNetworkPolicyItem>;

  export function getNetworkAccessPolicy(): Promise<UidNetworkAccessPolicy> {
    return new Promise<UidNetworkAccessPolicy>((resolve, reject) => {
      taskpool.execute((): Array<UidNetworkPolicyItem> => {
        return getAllNetworkAccessPoliciesSync();
      }).then((items: Any) => {
        let result: UidNetworkAccessPolicy = new Map<string, NetworkAccessPolicy>();
        let arr = items as Array<UidNetworkPolicyItem>;
        if (arr != null) {
          arr.forEach((item: UidNetworkPolicyItem): void => {
            let value: NetworkAccessPolicy = {
              allowWiFi: item.allowWiFi,
              allowCellular: item.allowCellular,
              alwaysAllowWiFi: item.alwaysAllowWiFi,
              alwaysAllowCellular: item.alwaysAllowCellular,
            };
            result.set(String(item.uid), value);
          });
        }
        resolve(result);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function getNetQuotaPoliciesSync(): Array<NetQuotaPolicy>;

  export function getNetQuotaPolicies(callback: AsyncCallback<Array<NetQuotaPolicy>>): void {
    let p1 = taskpool.execute((): Array<NetQuotaPolicy> => {
      return getNetQuotaPoliciesSync();
    })
    p1.then((content: Any) => {
      callback(null, content as Array<NetQuotaPolicy>);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  export function getNetQuotaPolicies(): Promise<Array<NetQuotaPolicy>> {
    return new Promise<Array<NetQuotaPolicy>>((resolve, reject) => {
      taskpool.execute((): Array<NetQuotaPolicy> => {
        return getNetQuotaPoliciesSync();
      }).then((content: Any) => {
        resolve(content as Array<NetQuotaPolicy>);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function setNetworkAccessPolicySync(uid: int, policy: NetworkAccessPolicy, reconfirmFlag: boolean): void;

  export function setNetworkAccessPolicy(uid: int, policy: NetworkAccessPolicy,
      isReconfirmed?: boolean): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        setNetworkAccessPolicySync(uid, policy, isReconfirmed ?? false);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function setNetQuotaPoliciesSync(quotaPolicies: Array<NetQuotaPolicy>): void;

  export function setNetQuotaPolicies(quotaPolicies: Array<NetQuotaPolicy>, callback: AsyncCallback<void>): void {
    taskpool.execute(() => {
      setNetQuotaPoliciesSync(quotaPolicies);
    }).then(() => callback(null, undefined),
      (err: Error): void => callback(err as BusinessError, undefined));
  }

  export function setNetQuotaPolicies(quotaPolicies: Array<NetQuotaPolicy>): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        setNetQuotaPoliciesSync(quotaPolicies);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

}
