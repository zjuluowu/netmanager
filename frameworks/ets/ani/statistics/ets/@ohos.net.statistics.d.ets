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


import type { AsyncCallback, Callback } from '@ohos.base';
import type connection from '@ohos.net.connection';
import { BusinessError }  from '@ohos.base';

export default namespace statistics {
  loadLibrary("statistics_ani")

  export type NetBearType = connection.NetBearType;

  export native function getIfaceRxBytesSync(nic: string): long;

  export function getIfaceRxBytes(nic: string, callback: AsyncCallback<long>): void {
      let p1 = taskpool.execute((): long => {
          return getIfaceRxBytesSync(nic);
      })
      p1.then((content: Any) => {
          callback(new BusinessError(), content as long);
      }, (err: Error): void => {
          callback(err as BusinessError, 0);
      });
  }

  export function getIfaceRxBytes(nic: string): Promise<long> {
      return new Promise<long>((resolve, reject) => {
          taskpool.execute((): long => {
          return getIfaceRxBytesSync(nic);
      }).then((content: Any) => {
          resolve(content as long);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
  });
  }

  export native function getIfaceTxBytesSync(nic: string): long;

  export function getIfaceTxBytes(nic: string, callback: AsyncCallback<long>): void {
      let p1 = taskpool.execute((): long => {
          return getIfaceTxBytesSync(nic);
      })
      p1.then((content: Any) => {
          callback(new BusinessError(), content as long);
      }, (err: Error): void => {
          callback(err as BusinessError, 0);
      });
  }

  export function getIfaceTxBytes(nic: string): Promise<long> {
      return new Promise<long>((resolve, reject) => {
          taskpool.execute((): long => {
          return getIfaceTxBytesSync(nic);
      }).then((content: Any) => {
          resolve(content as long);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
  });
  }

  export native function getCellularRxBytesSync(): long;

  export function getCellularRxBytes(callback: AsyncCallback<long>): void {
      let p1 = taskpool.execute((): long => {
          return getCellularRxBytesSync();
      })
      p1.then((content: Any) => {
          callback(new BusinessError(), content as long);
      }, (err: Error): void => {
          callback(err as BusinessError, 0);
      });
  }

  export function getCellularRxBytes(): Promise<long> {
      return new Promise<long>((resolve, reject) => {
          taskpool.execute((): long => {
          return getCellularRxBytesSync();
      }).then((content: Any) => {
          resolve(content as long);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
  });
  }

  export native function getCellularTxBytesSync(): long;

  export function getCellularTxBytes(callback: AsyncCallback<long>): void {
      let p1 = taskpool.execute((): long => {
          return getCellularTxBytesSync();
      })
      p1.then((content: Any) => {
          callback(new BusinessError(), content as long);
      }, (err: Error): void => {
          callback(err as BusinessError, 0);
      });
  }

  export function getCellularTxBytes(): Promise<long> {
      return new Promise<long>((resolve, reject) => {
          taskpool.execute((): long => {
          return getCellularTxBytesSync();
      }).then((content: Any) => {
          resolve(content as long);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
  });
  }

  export native function getAllRxBytesSync(): long;

  export function getAllRxBytes(callback: AsyncCallback<long>): void {
      let p1 = taskpool.execute((): long => {
          return getAllRxBytesSync();
      })
      p1.then((content: Any) => {
          callback(new BusinessError(), content as long);
      }, (err: Error): void => {
          callback(err as BusinessError, 0);
      });
  }

  export function getAllRxBytes(): Promise<long> {
      return new Promise<long>((resolve, reject) => {
          taskpool.execute((): long => {
          return getAllRxBytesSync();
      }).then((content: Any) => {
          resolve(content as long);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
  });
  }

  export native function getAllTxBytesSync(): long;

  export function getAllTxBytes(callback: AsyncCallback<long>): void {
      let p1 = taskpool.execute((): long => {
          return getAllTxBytesSync();
      })
      p1.then((content: Any) => {
          callback(new BusinessError(), content as long);
      }, (err: Error): void => {
          callback(err as BusinessError, 0);
      });
  }

  export function getAllTxBytes(): Promise<long> {
      return new Promise<long>((resolve, reject) => {
          taskpool.execute((): long => {
          return getAllTxBytesSync();
      }).then((content: Any) => {
          resolve(content as long);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
  });
  }

  export native function getUidRxBytesSync(uid: int): long;

  export function getUidRxBytes(uid: int, callback: AsyncCallback<long>): void {
      let p1 = taskpool.execute((): long => {
          return getUidRxBytesSync(uid);
      })
      p1.then((content: Any) => {
          callback(new BusinessError(), content as long);
      }, (err: Error): void => {
          callback(err as BusinessError, 0);
      });
  }

  export function getUidRxBytes(uid: int): Promise<long> {
      return new Promise<long>((resolve, reject) => {
          taskpool.execute((): long => {
          return getUidRxBytesSync(uid);
      }).then((content: Any) => {
          resolve(content as long);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
  });
  }

  export native function getUidTxBytesSync(uid: int): long;

  export function getUidTxBytes(uid: int, callback: AsyncCallback<long>): void {
      let p1 = taskpool.execute((): long => {
          return getUidTxBytesSync(uid);
      })
      p1.then((content: Any) => {
          callback(new BusinessError(), content as long);
      }, (err: Error): void => {
          callback(err as BusinessError, 0);
      });
  }

  export function getUidTxBytes(uid: int): Promise<long> {
      return new Promise<long>((resolve, reject) => {
        taskpool.execute((): long => {
        return getUidTxBytesSync(uid);
      }).then((content: Any) => {
        resolve(content as long);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export function on(type: 'netStatsChange', callback: Callback<NetStatsChangeInfo>): void {
    if (type == 'netStatsChange') {
      onNetStatsChangeSync(callback as Callback<NetStatsChangeInfo>);
    }
  }

  export function off(type: 'netStatsChange', callback?: Callback<NetStatsChangeInfo>): void {
    if (type == 'netStatsChange') {
      offNetStatsChangeSync(callback as Callback<NetStatsChangeInfo> | undefined);
    }
  }

  export native function onNetStatsChangeSync(callback: Callback<NetStatsChangeInfo>): void;
  export native function offNetStatsChangeSync(callback?: Callback<NetStatsChangeInfo>): void;

  export function onNetStatsChange(callback: Callback<NetStatsChangeInfo>): void {
    onNetStatsChangeSync(callback as Callback<NetStatsChangeInfo>);
  }

  export function offNetStatsChange(callback?: Callback<NetStatsChangeInfo>): void {
    offNetStatsChangeSync(callback as Callback<NetStatsChangeInfo> | undefined);
  }

  export native function getTrafficStatsByIfaceSync(ifaceInfo: IfaceInfo): NetStatsInfo;

  export function getTrafficStatsByIface(ifaceInfo: IfaceInfo, callback: AsyncCallback<NetStatsInfo>): void {
    let p1 = taskpool.execute((): NetStatsInfo => {
          return getTrafficStatsByIfaceSync(ifaceInfo);
      })
      p1.then((content: Any) => {
          callback(new BusinessError(), content as NetStatsInfo);
      }, (err: Error): void => {
          callback(err as BusinessError, new NetStatsInfoInner());
      });
  }

  export function getTrafficStatsByIface(ifaceInfo: IfaceInfo): Promise<NetStatsInfo> {
    return new Promise<NetStatsInfo>((resolve, reject) => {
        taskpool.execute((): NetStatsInfo => {
        return getTrafficStatsByIfaceSync(ifaceInfo);
      }).then((content: Any) => {
        resolve(content as NetStatsInfo);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function getTrafficStatsByUidSync(uidInfo: UidInfo): NetStatsInfo;
  export function getTrafficStatsByUid(uidInfo: UidInfo, callback: AsyncCallback<NetStatsInfo>): void {
    let p1 = taskpool.execute((): NetStatsInfo => {
          return getTrafficStatsByUidSync(uidInfo);
      })
      p1.then((content: Any) => {
          callback(new BusinessError(), content as NetStatsInfo);
      }, (err: Error): void => {
          callback(err as BusinessError, new NetStatsInfoInner());
      });
  }

  export function getTrafficStatsByUid(uidInfo: UidInfo): Promise<NetStatsInfo> {
    return new Promise<NetStatsInfo>((resolve, reject) => {
        taskpool.execute((): NetStatsInfo => {
        return getTrafficStatsByUidSync(uidInfo);
      }).then((content: Any) => {
        resolve(content as NetStatsInfo);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  export native function getSockfdRxBytesSync(sockfd: int): long;

  export function getSockfdRxBytes(sockfd: int, callback: AsyncCallback<long>): void {
    let p1 = taskpool.execute((): long => {
        return getSockfdRxBytesSync(sockfd);
    })
    p1.then((content: Any) => {
        callback(new BusinessError(), content as long);
    }, (err: Error): void => {
        callback(err as BusinessError, 0);
    });
  }

  export function getSockfdRxBytes(sockfd: int): Promise<long> {
    return new Promise<long>((resolve, reject) => {
          taskpool.execute((): long => {
          return getSockfdRxBytesSync(sockfd);
      }).then((content: Any) => {
          resolve(content as long);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
    });
  }

  export native function getSockfdTxBytesSync(sockfd: int): long;

  export function getSockfdTxBytes(sockfd: int, callback: AsyncCallback<long>): void {
    let p1 = taskpool.execute((): long => {
        return getSockfdTxBytesSync(sockfd);
    })
    p1.then((content: Any) => {
        callback(new BusinessError(), content as long);
    }, (err: Error): void => {
        callback(err as BusinessError, 0);
    });
  }

  export function getSockfdTxBytes(sockfd: int): Promise<long> {
    return new Promise<long>((resolve, reject) => {
          taskpool.execute((): long => {
          return getSockfdTxBytesSync(sockfd);
      }).then((content: Any) => {
          resolve(content as long);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
    });
  }

  export interface IfaceInfo {
    iface: string;
    startTime: int;
    endTime: int;
  }

  export interface UidInfo {
    ifaceInfo: IfaceInfo;
    uid: int;
  }

  export interface NetStatsInfo {
    rxBytes: long;
    txBytes: long;
    rxPackets: long;
    txPackets: long;
  }

  export class NetStatsInfoInner implements NetStatsInfo {
    rxBytes: long;
    txBytes: long;
    rxPackets: long;
    txPackets: long;
  }

  export interface NetStatsChangeInfo {
    iface: string;
    uid?: int;
  }

  export type NetStatsInfoSequence = Array<NetStatsInfoSequenceItem>;

  export interface NetStatsInfoSequenceItem {
    startTime: int;
    endTime: int;
    info: NetStatsInfo;
  }

  export class NetStatsInfoSequenceItemInner implements NetStatsInfoSequenceItem {
    startTime: int;
    endTime: int;
    info: NetStatsInfo;
  }

  export type UidNetStatsInfo = Record<int, NetStatsInfo>;

  export enum TrafficPlanParam {
    DISPLAY_TRAFFIC_SWITCH = 1,
    UNLIMIT_TRAFFIC_SWITCH = 2,
    TRAFFIC_LIMIT = 3,
    START_DATE = 4,
    OVER_LIMIT_BEHAVIOR = 5,
    MONTHLY_LIMIT_PERCENTAGE = 6,
    DAILY_LIMIT_PERCENTAGE = 7,
  }

  export interface NetworkInfo {
    type: NetBearType;
    startTime: int;
    endTime: int;
    simId?: int;
  }

  export native function getTrafficStatsByNetworkSync(networkInfo: NetworkInfo): UidNetStatsInfo;

  export function getTrafficStatsByNetwork(networkInfo: NetworkInfo): Promise<UidNetStatsInfo> {
    return new Promise<UidNetStatsInfo>((resolve, reject) => {
          taskpool.execute((): UidNetStatsInfo => {
            return getTrafficStatsByNetworkSync(networkInfo);
      }).then((content: Any) => {
          resolve(content as UidNetStatsInfo);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
    });
  }

  export native function getTrafficStatsByUidNetworkSync(uid: int, networkInfo: NetworkInfo): NetStatsInfoSequence;

  export function getTrafficStatsByUidNetwork(uid: int, networkInfo: NetworkInfo): Promise<NetStatsInfoSequence> {
    return new Promise<NetStatsInfoSequence>((resolve, reject) => {
          taskpool.execute((): NetStatsInfoSequence => {
          return getTrafficStatsByUidNetworkSync(uid, networkInfo);
      }).then((content: Any) => {
          resolve(content as NetStatsInfoSequence);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
    });
  }

  // ==================== New Interfaces Below ====================

  // ==================== updateStatsData ====================

  export native function updateStatsDataSync(): void;

  export function updateStatsData(): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        updateStatsDataSync();
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  // ==================== getSelfTrafficStats ====================

  export native function getSelfTrafficStatsSync(networkInfo: NetworkInfo): NetStatsInfo;

  export function getSelfTrafficStats(networkInfo: NetworkInfo): Promise<NetStatsInfo> {
    return new Promise<NetStatsInfo>((resolve, reject) => {
      taskpool.execute((): NetStatsInfo => {
        return getSelfTrafficStatsSync(networkInfo);
      }).then((content: Any) => {
        resolve(content as NetStatsInfo);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  // ==================== updateIfacesStats ====================

  export native function updateIfacesStatsSync(iface: string, start: int, end: int,
      stats: NetStatsInfo): void;

  export function updateIfacesStats(iface: string, start: int, end: int, stats: NetStatsInfo): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        updateIfacesStatsSync(iface, start, end, stats);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
  }

  // ==================== setCalibrationTraffic ====================

  export native function setCalibrationTrafficSync(simId: int, remainTraffic: long, totalTraffic: long): void;

  export function setCalibrationTraffic(simId: int, remainTraffic: long, totalTraffic?: long): Promise<void> {
    let tt: long = totalTraffic ?? 0;
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        setCalibrationTrafficSync(simId, remainTraffic, tt);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
        if (err instanceof BusinessError) {
          reject(err);
        } else {
          reject(new BusinessError(err.code, err.message));
        }
      });
    });
  }

  // ==================== getTrafficPlanInfo ====================

  export native function getTrafficPlanInfoSync(simId: int, param: int): long;

  export function getTrafficPlanInfo(simId: int, param: int): Promise<long> {
    return new Promise<long>((resolve, reject) => {
      taskpool.execute((): long => {
        return getTrafficPlanInfoSync(simId, param);
      }).then((content: Any) => {
        resolve(content as long);
      }, (err: Error): void => {
         if (err instanceof BusinessError) {
          reject(err);
        } else {
          reject(new BusinessError(err.code, err.message));
        }
      });
    });
  }

  // ==================== setTrafficPlanInfo ====================

  export native function setTrafficPlanInfoSync(simId: int, param: int, value: long): void;

  export function setTrafficPlanInfo(simId: int, param: int, value: long): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        setTrafficPlanInfoSync(simId, param, value);
      }).then(() => {
        resolve(undefined);
      }, (err: Error): void => {
         if (err instanceof BusinessError) {
          reject(err);
        } else {
          reject(new BusinessError(err.code, err.message));
        }
      });
    });
  }
}
