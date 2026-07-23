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

import type { AsyncCallback, Callback } from '@ohos.base';
import { BusinessError } from '@ohos.base';

export default namespace connection {
  loadLibrary("connection_ani");

  // NOTE: ArkTS forbids 'any' type annotations (ESE153545), so err must remain
  // typed as Error. In practice, taskpool.execute wraps all throwables in Error
  // instances, so non-Error throws are not expected at runtime in the ArkTS env.
  function toBusinessError(err: Error): BusinessError {
    let businessErr = new BusinessError();
    businessErr.message = err.message;
    businessErr.stack = err.stack ?? '';
    businessErr.cause = err;
    if (err instanceof BusinessError) {
      businessErr.code = err.code;
    }
    return businessErr;
  }

  export class Cleaner {
    private ptr: long = 0
    constructor(ptr: long) {
        this.ptr = ptr
    }
    native clean(): void
  }

  let destroyRegister: FinalizationRegistry<Cleaner> = new FinalizationRegistry<Cleaner>((cleaner: Cleaner) => {cleaner.clean()})
  let unregisterToken: object = new object()

  export native function createNetConnection(netSpecifier?: NetSpecifier, timeout?: int): NetConnection;

  export function getDefaultNet(callback: AsyncCallback<NetHandle>): void {
    let p1 = taskpool.execute((): NetHandle => {
        return getDefaultNetSync();
    })
    p1.then((content: Any) => {
        callback(new BusinessError(), content as NetHandle);
    }, (err: Error): void => {
        callback(err as BusinessError, new NetHandleInner);
    });
  }

  export function getDefaultNet(): Promise<NetHandle> {
    return new Promise<NetHandle>((resolve, reject) => {
        taskpool.execute((): NetHandle => {
            return getDefaultNetSync();
        }).then((content: Any) => {
            resolve(content as NetHandle);
        }, (err: Error): void => {
            reject(err as BusinessError);
        });
    });
  }

  export native function getDefaultNetSync(): NetHandle;

  export function getAllNets(callback: AsyncCallback<Array<NetHandle>>): void {
    let p1 = taskpool.execute((): Array<NetHandle> => {
        return getAllNetsSync();
    })
    p1.then((content: Any) => {
        callback(new BusinessError(), content as Array<NetHandle>);
    }, (err: Error): void => {
        callback(err as BusinessError, new Array<NetHandle>());
    });
  }

  export function getAllNets(): Promise<Array<NetHandle>> {
    return new Promise<Array<NetHandle>>((resolve, reject) => {
        taskpool.execute((): Array<NetHandle> => {
            return getAllNetsSync();
        }).then((content: Any) => {
            resolve(content as Array<NetHandle>);
        }, (err: Error): void => {
            reject(err as BusinessError);
        });
    });
  }

  export native function getAllNetsSync(): Array<NetHandle>;

  export function getConnectionProperties(netHandle: NetHandle, callback: AsyncCallback<ConnectionProperties>): void {
    let p1 = taskpool.execute((): ConnectionProperties => {
        return getConnectionPropertiesSync(netHandle);
    })
    p1.then((content: Any) => {
        callback(new BusinessError(), content as ConnectionProperties);
    }, (err: Error): void => {
        callback(err as BusinessError, new ConnectionPropertiesInner());
    });
  }

  export function getConnectionProperties(netHandle: NetHandle): Promise<ConnectionProperties> {
    return new Promise<ConnectionProperties>((resolve, reject) => {
        taskpool.execute((): ConnectionProperties => {
            return getConnectionPropertiesSync(netHandle);
        }).then((content: Any) => {
            resolve(content as ConnectionProperties);
        }, (err: Error): void => {
             reject(err as BusinessError);
        });
    });
  }

  export native function getConnectionPropertiesSync(netHandle: NetHandle): ConnectionProperties;

  export function getNetCapabilities(netHandle: NetHandle, callback: AsyncCallback<NetCapabilities>): void {
    let p1 = taskpool.execute((): NetCapabilities => {
        return getNetCapabilitiesSync(netHandle);
    })
    p1.then((content: Any) => {
        callback(new BusinessError(), content as NetCapabilities);
    }, (err: Error): void => {
        callback(err as BusinessError, new NetCapabilitiesInner);
    });
  }

  export function getNetCapabilities(netHandle: NetHandle): Promise<NetCapabilities> {
    return new Promise<NetCapabilities>((resolve, reject) => {
        taskpool.execute((): NetCapabilities => {
            return getNetCapabilitiesSync(netHandle);
        }).then((content: Any) => {
            resolve(content as NetCapabilities);
        }, (err: Error): void => {
            reject(err as BusinessError);
        });
    });
  }

  export native function getNetCapabilitiesSync(netHandle: NetHandle): NetCapabilities;

  export function isDefaultNetMetered(callback: AsyncCallback<boolean>): void {
    let p1 = taskpool.execute((): boolean => {
        return isDefaultNetMeteredSync();
    })
    p1.then((content: Any) => {
        callback(new BusinessError(), content as boolean);
    }, (err: Error): void => {
        callback(err as BusinessError, false);
    });
  }

  export function isDefaultNetMetered(): Promise<boolean> {
    return new Promise<boolean>((resolve, reject) => {
        taskpool.execute((): boolean => {
            return isDefaultNetMeteredSync();
        }).then((content: Any) => {
            resolve(content as boolean);
        }, (err: Error): void => {
            reject(err as BusinessError);
        });
    });
  }

  export native function isDefaultNetMeteredSync(): boolean;

  export function hasDefaultNet(callback: AsyncCallback<boolean>): void {
    let p1 = taskpool.execute((): boolean => {
        return hasDefaultNetSync();
    })
    p1.then((content: Any) => {
        callback(new BusinessError(), content as boolean);
    }, (err: Error): void => {
        callback(err as BusinessError, false);
    });
  }

  export function hasDefaultNet(): Promise<boolean> {
    return new Promise<boolean>((resolve, reject) => {
        taskpool.execute((): boolean => {
            return hasDefaultNetSync();
        }).then((content: Any) => {
            resolve(content as boolean);
        }, (err: Error): void => {
            reject(err as BusinessError);
        });
    });
  }

  export native function hasDefaultNetSync(): boolean;

  export native function enableAirplaneModeSync(): void;

  export function enableAirplaneMode(callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        enableAirplaneModeSync();
    })
    p1.then(() => {
        callback(new BusinessError(), undefined);
    }, (err: Error): void => {
        callback(err as BusinessError, undefined);
    });
  }

  export function enableAirplaneMode(): Promise<void> {
    return new Promise<void>((resolve, reject) => {
        taskpool.execute(() => {
            enableAirplaneModeSync();
        }).then(() => {
            resolve(undefined);
        }, (err: Error): void => {
            reject(err as BusinessError);
        });
    });
  }

  export native function disableAirplaneModeSync(): void;

  export function disableAirplaneMode(callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        disableAirplaneModeSync();
    })
    p1.then(() => {
        callback(new BusinessError(), undefined);
    }, (err: Error): void => {
         callback(err as BusinessError, undefined);
    });
  }

  export function disableAirplaneMode(): Promise<void> {
    return new Promise<void>((resolve, reject) => {
        taskpool.execute(() => {
            disableAirplaneModeSync();
        }).then((content: Any) => {
            resolve(undefined);
        }, (err: Error): void => {
             reject(err as BusinessError);
        });
    });
  }

  export function reportNetConnected(netHandle: NetHandle, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        reportNetConnectedSync(netHandle);
    })
    p1.then(() => {
        callback(new BusinessError(), undefined);
    }, (err: Error): void => {
        callback(err as BusinessError, undefined);
    });
  }

  export function reportNetConnected(netHandle: NetHandle): Promise<void> {
    return new Promise<void>((resolve, reject) => {
        taskpool.execute(() => {
            reportNetConnectedSync(netHandle);
        }).then((content: Any) => {
            resolve(undefined);
        }, (err: Error): void => {
             reject(err as BusinessError);
        });
    });
  }

  export native function reportNetConnectedSync(netHandle: NetHandle): void;

  export function reportNetDisconnected(netHandle: NetHandle, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        reportNetDisconnectedSync(netHandle);
    })
    p1.then(() => {
        callback(new BusinessError(), undefined);
    }, (err: Error): void => {
        callback(err as BusinessError, undefined);
    });
  }

  export function reportNetDisconnected(netHandle: NetHandle): Promise<void> {
    return new Promise<void>((resolve, reject) => {
        taskpool.execute(() => {
            reportNetDisconnectedSync(netHandle);
        }).then((content: Any) => {
            resolve(undefined);
        }, (err: Error): void => {
             reject(err as BusinessError);
        });
    });
  }

  export native function reportNetDisconnectedSync(netHandle: NetHandle): void;

  export function getAddressesByName(host: string, callback: AsyncCallback<Array<NetAddress>>): void {
    let p1 = taskpool.execute((): Array<NetAddress> => {
        return getAddressesByNameSync(host);
    })
    p1.then((content: Any) => {
        callback(new BusinessError(), content as Array<NetAddress>);
    }, (err: Error): void => {
        callback(err as BusinessError, undefined);
    });
  }

  export function getAddressesByName(host: string): Promise<Array<NetAddress>> {
    return new Promise<Array<NetAddress>>((resolve, reject) => {
        taskpool.execute((): Array<NetAddress> => {
            return getAddressesByNameSync(host);
        }).then((content: Any) => {
            resolve(content as Array<NetAddress>);
        }, (err: Error): void => {
             reject(err as BusinessError);
        });
    });
  }

  export native function getAddressesByNameSync(host: string): Array<NetAddress>;

  export function getAddressesByNameWithOptions(host: string, option?: QueryOptions): Promise<Array<NetAddress>> {
    return new Promise<Array<NetAddress>>((resolve, reject) => {
      let opt: QueryOptions = option ?? {};
      taskpool.execute((): Array<NetAddress> => {
          return getAddressesByNameWithOptionsSync(host, opt);
      }).then((content: Any) => {
          resolve(content as Array<NetAddress>);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function getAddressesByNameWithOptionsSync(host: string, option?: QueryOptions): Array<NetAddress>;

  export function getAppNet(callback: AsyncCallback<NetHandle>): void {
    let p1 = taskpool.execute((): NetHandle => {
        return getAppNetSync();
    })
    p1.then((content: Any) => {
        callback(new BusinessError(), content as NetHandle);
    }, (err: Error): void => {
        callback(err as BusinessError, new NetHandleInner);
    });
  }

  export function getAppNet(): Promise<NetHandle> {
    return new Promise<NetHandle>((resolve, reject) => {
      taskpool.execute((): NetHandle => {
          return getAppNetSync();
      }).then((content: Any) => {
          resolve(content as NetHandle);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
    });
  }

  export native function getAppNetSync(): NetHandle;

  export function setAppNet(netHandle: NetHandle, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        setAppNetSync(netHandle);
    })
    p1.then(() => {
        callback(new BusinessError(), undefined);
    }, (err: Error): void => {
        callback(err as BusinessError, undefined);
    });
  }

  export function setAppNet(netHandle: NetHandle): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          setAppNetSync(netHandle);
      }).then(() => {
          resolve(undefined);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
    });
  }

  export native function setAppNetSync(netHandle: NetHandle): void;

  function getDefaultHttpProxy(callback: AsyncCallback<HttpProxy>): void {
    let p1 = taskpool.execute((): HttpProxy => {
        return getDefaultHttpProxySync();
    })
    p1.then((content: Any) => {
        callback(new BusinessError(), content as HttpProxy);
    }, (err: Error): void => {
        callback(err as BusinessError, new HttpProxyInner);
    });
  }

  function getDefaultHttpProxy(): Promise<HttpProxy> {
    return new Promise<HttpProxy>((resolve, reject) => {
        taskpool.execute((): HttpProxy => {
            return getDefaultHttpProxySync();
        }).then((content: Any) => {
            resolve(content as HttpProxy);
        }, (err: Error): void => {
            reject(err as BusinessError);
        });
    });
  }

  export native function getDefaultHttpProxySync(): HttpProxy;

  export function getGlobalHttpProxy(callback: AsyncCallback<HttpProxy>): void {
    let p1 = taskpool.execute((): HttpProxy => {
        return getGlobalHttpProxySync();
    })
    p1.then((content: Any) => {
        callback(new BusinessError(), content as HttpProxy);
    }, (err: Error): void => {
        callback(err as BusinessError, new HttpProxyInner);
    });
  }

  export function getGlobalHttpProxy(): Promise<HttpProxy> {
    return new Promise<HttpProxy>((resolve, reject) => {
        taskpool.execute((): HttpProxy => {
            return getGlobalHttpProxySync();
        }).then((content: Any) => {
            resolve(content as HttpProxy);
        }, (err: Error): void => {
            reject(err as BusinessError);
        });
    });
  }

  export native function getGlobalHttpProxySync(): HttpProxy;

  export native function setAppHttpProxy(httpProxy: HttpProxy): void;

  export function setGlobalHttpProxy(httpProxy: HttpProxy, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        setGlobalHttpProxySync(httpProxy);
    })
    p1.then(() => {
        callback(new BusinessError(), undefined);
    }, (err: Error): void => {
        callback(err as BusinessError, undefined);
    });
  }

  export function setGlobalHttpProxy(httpProxy: HttpProxy): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          setGlobalHttpProxySync(httpProxy);
      }).then(() => {
          resolve(undefined);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
    });
  }

  export native function setGlobalHttpProxySync(httpProxy: HttpProxy): void;

  export native function setPacUrl(pacUrl: string): void;

  export native function getPacUrl(): string;

  export function setPacFileUrl(pacFileUrl: string): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
        setPacFileUrlSync(pacFileUrl);
      }).then(() => {
        resolve(undefined);
      }).catch((err: Error) => {
        reject(toBusinessError(err));
      });
    });
  }

  export native function setPacFileUrlSync(pacFileUrl: string): void;

  export function findProxyForUrl(url: string, callback: AsyncCallback<string>): void {
    let p1 = taskpool.execute((): string => {
        return findProxyForUrlSync(url);
    })
    p1.then((content: Any) => {
        callback(null, content as string);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function findProxyForUrl(url: string): Promise<string> {
    return new Promise<string>((resolve, reject) => {
      taskpool.execute((): string => {
          return findProxyForUrlSync(url);
      }).then((content: Any) => {
          resolve(content as string);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function findProxyForUrlSync(url: string): string;

  export function addCustomDnsRule(host: string, ip: Array<string>, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        addCustomDnsRuleSync(host, ip);
    })
    p1.then(() => {
        callback(new BusinessError(), undefined);
    }, (err: Error): void => {
        callback(err as BusinessError, undefined);
    });
  }

  export function addCustomDnsRule(host: string, ip: Array<string>): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          addCustomDnsRuleSync(host, ip);
      }).then(() => {
          resolve(undefined);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
    });
  }

  export native function addCustomDnsRuleSync(host: string, ip: Array<string>): void;

  export function removeCustomDnsRule(host: string, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        removeCustomDnsRuleSync(host);
    })
    p1.then(() => {
        callback(new BusinessError(), undefined);
    }, (err: Error): void => {
        callback(err as BusinessError, undefined);
    });
  }

  export function removeCustomDnsRule(host: string): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          removeCustomDnsRuleSync(host);
      }).then(() => {
          resolve(undefined);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
    });
  }

  export native function removeCustomDnsRuleSync(host: string): void;

  export function clearCustomDnsRules(callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        clearCustomDnsRulesSync();
    })
    p1.then(() => {
        callback(new BusinessError(), undefined);
    }, (err: Error): void => {
        callback(err as BusinessError, undefined);
    });
  }

  export function clearCustomDnsRules(): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          clearCustomDnsRulesSync();
      }).then(() => {
          resolve(undefined);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
    });
  }

  export native function clearCustomDnsRulesSync(): void;

  export function refreshGlobalHttpProxy(callback: AsyncCallback<HttpProxy>): void {
    let p1 = taskpool.execute((): HttpProxy => {
        return refreshGlobalHttpProxySync();
    })
    p1.then((content: Any) => {
        callback(null, content as HttpProxy);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function refreshGlobalHttpProxy(): Promise<HttpProxy> {
    return new Promise<HttpProxy>((resolve, reject) => {
      taskpool.execute((): HttpProxy => {
          return refreshGlobalHttpProxySync();
      }).then((content: Any) => {
          resolve(content as HttpProxy);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function refreshGlobalHttpProxySync(): HttpProxy;

  export function createVlanInterface(ifName: string, vlanId: int, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        createVlanInterfaceSync(ifName, vlanId);
    })
    p1.then(() => {
        callback(null, undefined);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function createVlanInterface(ifName: string, vlanId: int): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          createVlanInterfaceSync(ifName, vlanId);
      }).then(() => {
          resolve(undefined);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function createVlanInterfaceSync(ifName: string, vlanId: int): void;

  export function destroyVlanInterface(ifName: string, vlanId: int, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        destroyVlanInterfaceSync(ifName, vlanId);
    })
    p1.then(() => {
        callback(null, undefined);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function destroyVlanInterface(ifName: string, vlanId: int): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          destroyVlanInterfaceSync(ifName, vlanId);
      }).then(() => {
          resolve(undefined);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function destroyVlanInterfaceSync(ifName: string, vlanId: int): void;

  export function addVlanIp(ifName: string, vlanId: int, address: LinkAddress, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        addVlanIpSync(ifName, vlanId, address.address.address, address.prefixLength);
    })
    p1.then(() => {
        callback(null, undefined);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function addVlanIp(ifName: string, vlanId: int, address: LinkAddress): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          addVlanIpSync(ifName, vlanId, address.address.address, address.prefixLength);
      }).then(() => {
          resolve(undefined);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function addVlanIpSync(ifName: string, vlanId: int, ip: string, mask: int): void;

  export function deleteVlanIp(ifName: string, vlanId: int, address: LinkAddress, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        deleteVlanIpSync(ifName, vlanId, address.address.address, address.prefixLength);
    })
    p1.then(() => {
        callback(null, undefined);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function deleteVlanIp(ifName: string, vlanId: int, address: LinkAddress): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          deleteVlanIpSync(ifName, vlanId, address.address.address, address.prefixLength);
      }).then(() => {
          resolve(undefined);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function deleteVlanIpSync(ifName: string, vlanId: int, ip: string, mask: int): void;

  export function getSystemNetPortStates(callback: AsyncCallback<NetPortStatesInfo>): void {
    let p1 = taskpool.execute((): NetPortStatesInfo => {
        return getSystemNetPortStatesSync();
    })
    p1.then((content: Any) => {
        callback(null, content as NetPortStatesInfo);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function getSystemNetPortStates(): Promise<NetPortStatesInfo> {
    return new Promise<NetPortStatesInfo>((resolve, reject) => {
      taskpool.execute((): NetPortStatesInfo => {
          return getSystemNetPortStatesSync();
      }).then((content: Any) => {
          resolve(content as NetPortStatesInfo);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function getSystemNetPortStatesSync(): NetPortStatesInfo;

  export function getIpNeighTable(callback: AsyncCallback<Array<NetIpMacInfo>>): void {
    let p1 = taskpool.execute((): Array<NetIpMacInfo> => {
        return getIpNeighTableSync();
    })
    p1.then((content: Any) => {
        callback(null, content as Array<NetIpMacInfo>);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function getIpNeighTable(): Promise<Array<NetIpMacInfo>> {
    return new Promise<Array<NetIpMacInfo>>((resolve, reject) => {
      taskpool.execute((): Array<NetIpMacInfo> => {
          return getIpNeighTableSync();
      }).then((content: Any) => {
          resolve(content as Array<NetIpMacInfo>);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function getIpNeighTableSync(): Array<NetIpMacInfo>;

  export function getConnectOwnerUid(protocol: ProtocolType, local: NetAddress, remote: NetAddress,
    callback: AsyncCallback<int>): void {
    let family: FamilyType = FamilyType.fromValue(local.family ?? 1);
    let param = new NetConnInfoParamInner(protocol, family, local.address, local.port ?? 0,
      remote.address, remote.port ?? 0);
    let p1 = taskpool.execute((): int => {
        return getConnectOwnerUidSyncImpl(param);
    })
    p1.then((content: Any) => {
        callback(null, content as int);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function getConnectOwnerUid(protocol: ProtocolType, local: NetAddress, remote: NetAddress): Promise<int> {
    let family: FamilyType = FamilyType.fromValue(local.family ?? 1);
    let param = new NetConnInfoParamInner(protocol, family, local.address, local.port ?? 0,
      remote.address, remote.port ?? 0);
    return new Promise<int>((resolve, reject) => {
      taskpool.execute((): int => {
          return getConnectOwnerUidSyncImpl(param);
      }).then((content: Any) => {
          resolve(content as int);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export function getConnectOwnerUidSync(protocol: ProtocolType, local: NetAddress, remote: NetAddress): int {
    let family: FamilyType = FamilyType.fromValue(local.family ?? 1);
    let param = new NetConnInfoParamInner(protocol, family, local.address, local.port ?? 0,
      remote.address, remote.port ?? 0);
    return getConnectOwnerUidSyncImpl(param);
  }

  export native function getConnectOwnerUidSyncImpl(param: NetConnInfoParamInner): int;

  export function getDnsUnicode(host: string, flag?: ConversionProcess): string {
    return getDnsUnicodeSync(host, flag ?? ConversionProcess.NO_CONFIGURATION);
  }

  export native function getDnsUnicodeSync(host: string, conversionProcess: ConversionProcess): string;

  export function getDnsAscii(host: string, flag?: ConversionProcess): string {
    return getDnsAsciiSync(host, flag ?? ConversionProcess.NO_CONFIGURATION);
  }

  export native function getDnsAsciiSync(host: string, conversionProcess: ConversionProcess): string;

  export function setInterfaceUp(iface: string, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        setInterfaceUpSync(iface);
    })
    p1.then(() => {
        callback(null, undefined);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function setInterfaceUp(iface: string): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          setInterfaceUpSync(iface);
      }).then(() => {
          resolve(undefined);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function setInterfaceUpSync(iface: string): void;

  export function queryProbeResult(destination: string, duration: int, callback: AsyncCallback<ProbeResultInfo>): void {
    let p1 = taskpool.execute((): ProbeResultInfo => {
        return queryProbeResultSync(destination, duration);
    })
    p1.then((content: Any) => {
        callback(null, content as ProbeResultInfo);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function queryProbeResult(destination: string, duration: int): Promise<ProbeResultInfo> {
    return new Promise<ProbeResultInfo>((resolve, reject) => {
      taskpool.execute((): ProbeResultInfo => {
          return queryProbeResultSync(destination, duration);
      }).then((content: Any) => {
          resolve(content as ProbeResultInfo);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function queryProbeResultSync(destination: string, duration: int): ProbeResultInfo;

  export function queryTraceRoute(destination: string, callback: AsyncCallback<Array<TraceRouteInfo>>,
    option?: TraceRouteOptions): void {
    let maxJump = option?.maxJumpNumber ?? 30;
    let pktType = option?.packetsType ?? PacketsType.NETCONN_PACKETS_ICMP;
    let p1 = taskpool.execute((): Array<TraceRouteInfo> => {
        return queryTraceRouteSync(destination, maxJump, pktType);
    })
    p1.then((content: Any) => {
        callback(null, content as Array<TraceRouteInfo>);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function queryTraceRoute(destination: string, option?: TraceRouteOptions): Promise<Array<TraceRouteInfo>> {
    return new Promise<Array<TraceRouteInfo>>((resolve, reject) => {
      let maxJump = option?.maxJumpNumber ?? 30;
      let pktType = option?.packetsType ?? PacketsType.NETCONN_PACKETS_ICMP;
      taskpool.execute((): Array<TraceRouteInfo> => {
          return queryTraceRouteSync(destination, maxJump, pktType);
      }).then((content: Any) => {
          resolve(content as Array<TraceRouteInfo>);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function queryTraceRouteSync(destination: string, maxJumpNumber: int, packetsType: PacketsType): Array<TraceRouteInfo>;

  export function getProxyMode(callback: AsyncCallback<ProxyMode>): void {
    let p1 = taskpool.execute((): ProxyMode => {
        return getProxyModeSync();
    })
    p1.then((content: Any) => {
        callback(null, content as ProxyMode);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function getProxyMode(): Promise<ProxyMode> {
    return new Promise<ProxyMode>((resolve, reject) => {
      taskpool.execute((): ProxyMode => {
          return getProxyModeSync();
      }).then((content: Any) => {
          resolve(content as ProxyMode);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function getProxyModeSync(): ProxyMode;

  export function setProxyMode(mode: ProxyMode, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        setProxyModeSync(mode);
    })
    p1.then(() => {
        callback(null, undefined);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function setProxyMode(mode: ProxyMode): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          setProxyModeSync(mode);
      }).then(() => {
          resolve(undefined);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function setProxyModeSync(mode: ProxyMode): void;

  export function getPacFileUrl(): string {
    return getPacFileUrlSync();
  }

  export native function getPacFileUrlSync(): string;

  export function setNetExtAttribute(netHandle: NetHandle, netExtAttribute: string, callback: AsyncCallback<void>): void {
    let p1 = taskpool.execute(() => {
        setNetExtAttributeSync(netHandle, netExtAttribute);
    })
    p1.then(() => {
        callback(null, undefined);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function setNetExtAttribute(netHandle: NetHandle, netExtAttribute: string): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          setNetExtAttributeSync(netHandle, netExtAttribute);
      }).then(() => {
          resolve(undefined);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function setNetExtAttributeSync(netHandle: NetHandle, netExtAttribute: string): void;

  export function getNetExtAttribute(netHandle: NetHandle, callback: AsyncCallback<string>): void {
    let p1 = taskpool.execute((): string => {
        return getNetExtAttributeSync(netHandle);
    })
    p1.then((content: Any) => {
        callback(null, content as string);
    }).catch((err: Error) => {
        callback(toBusinessError(err), undefined);
    });
  }

  export function getNetExtAttribute(netHandle: NetHandle): Promise<string> {
    return new Promise<string>((resolve, reject) => {
      taskpool.execute((): string => {
          return getNetExtAttributeSync(netHandle);
      }).then((content: Any) => {
          resolve(content as string);
      }).catch((err: Error) => {
          reject(toBusinessError(err));
      });
    });
  }

  export native function getNetExtAttributeSync(netHandle: NetHandle): string;

  export native function factoryResetSync(): void;

  export function factoryReset(): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      taskpool.execute(() => {
          factoryResetSync();
      }).then(() => {
          resolve(undefined);
      }, (err: Error): void => {
          reject(err as BusinessError);
      });
    });
  }

  export interface NetConnection {
      onNetAvailable(callback: Callback<NetHandle>): void;

      onNetCapabilitiesChange(callback: Callback<NetCapabilityInfo>): void;

      onNetConnectionPropertiesChange(callback: Callback<NetConnectionPropertyInfo>): void;

      onNetLost(callback: Callback<NetHandle>): void;

      onNetUnavailable(callback: Callback<void>): void;

      onNetBlockStatusChange(callback: Callback<NetBlockStatusInfo>): void;

      register(callback: AsyncCallback<void>): void;

      unregister(callback: AsyncCallback<void>): void;
  }

  class NetConnectionInner implements NetConnection {
    private nativePtr: long = 0;
    private cleaner: Cleaner | null = null;

    constructor(context: long) {
        if (this.nativePtr == 0) {
            this.nativePtr = context;
        }
        this.registerCleaner(this.nativePtr);
    }

    registerCleaner(ptr: long): void {
        this.cleaner = new Cleaner(ptr);
        destroyRegister.register(this, this.cleaner!, unregisterToken);
    }
    unregisterCleaner(): void {
        destroyRegister.unregister(unregisterToken);
    }

    native onNetAvailableInner(callback: Callback<NetHandle>): void;
    native onNetBlockStatusChangeInner(callback: Callback<NetBlockStatusInfo>): void;
    native onNetCapabilitiesChangeInner(callback: Callback<NetCapabilityInfo>): void;
    native onNetConnectionPropertiesChangeInner(callback: Callback<NetConnectionPropertyInfo>): void;
    native onNetLostInner(callback: Callback<NetHandle>): void;
    native onNetUnavailableInner(callback: Callback<void>): void;

    onNetLost(callback: Callback<NetHandle>): void {
      this.onNetLostInner(callback)
    }

    onNetUnavailable(callback: Callback<void>): void {
      this.onNetUnavailableInner(callback)
    }

    onNetBlockStatusChange(callback: Callback<NetBlockStatusInfo>): void {
      this.onNetBlockStatusChangeInner(callback)
    }

    onNetAvailable(callback: Callback<NetHandle>): void {
      this.onNetAvailableInner(callback)
    }

    onNetCapabilitiesChange(callback: Callback<NetCapabilityInfo>): void {
      this.onNetCapabilitiesChangeInner(callback)
    }

    onNetConnectionPropertiesChange(callback: Callback<NetConnectionPropertyInfo>): void {
      this.onNetConnectionPropertiesChangeInner(callback)
    }

    native registerSync(): void;
    register(callback: AsyncCallback<void>): void {
      let p1 = taskpool.execute(() => {
          this.registerSync();
      })
      p1.then(() => {
          callback(new BusinessError(), undefined);
      }, (err: Error): void => {
          callback(err as BusinessError, undefined);
      });
    }

    native unregisterSync(): void;

    unregister(callback: AsyncCallback<void>): void {
      let p1 = taskpool.execute(() => {
          this.unregisterSync();
      })
      p1.then(() => {
          callback(new BusinessError(), undefined);
      }, (err: Error): void => {
          callback(err as BusinessError, undefined);
      });
    }
  }

  export interface NetSpecifier {
    netCapabilities: NetCapabilities;

    bearerPrivateIdentifier?: string;
  }

  export interface NetCapabilityInfo {
    netHandle: NetHandle;

    netCap: NetCapabilities;
  }

  export class NetCapabilityInfoInner implements NetCapabilityInfo {
    netHandle: NetHandle;

    netCap: NetCapabilities;
  }

  export interface NetHandle {
    netId: int;

    getAddressesByName(host: string, callback: AsyncCallback<Array<NetAddress>>): void;

    getAddressesByName(host: string): Promise<Array<NetAddress>>;

    getAddressByName(host: string, callback: AsyncCallback<NetAddress>): void;

    getAddressByName(host: string): Promise<NetAddress>;
  }

  export class NetHandleInner implements NetHandle {
    netId: int;
    static { loadLibrary("connection_ani"); }

    native getAddressByNameSyncWithHandle(host: string): NetAddress;

    getAddressByName(host: string, callback: AsyncCallback<NetAddress>): void {
      let p1 = taskpool.execute((): NetAddress => {
        return this.getAddressByNameSyncWithHandle(host);
      })
      p1.then((content: Any) => {
          callback(new BusinessError(), content as NetAddress);
      }, (err: Error): void => {
          callback(err as BusinessError, new NetAddressInner);
      });
    }

    getAddressByName(host: string): Promise<NetAddress> {
      return new Promise<NetAddress>((resolve, reject) => {
        taskpool.execute((): NetAddress => {
            return this.getAddressByNameSyncWithHandle(host);
        }).then((content: Any) => {
            resolve(content as NetAddress);
        }, (err: Error): void => {
            reject(err as BusinessError);
        });
      });
    }

    native getAddressesByNameSyncWithHandle(host: string): Array<NetAddress>;

    getAddressesByName(host: string, callback: AsyncCallback<Array<NetAddress>>): void {
      let p1 = taskpool.execute((): Array<NetAddress> => {
        return this.getAddressesByNameSyncWithHandle(host);
      })
      p1.then((content: Any) => {
          callback(new BusinessError(), content as Array<NetAddress>);
      }, (err: Error): void => {
          callback(err as BusinessError, undefined);
      });
    }

    getAddressesByName(host: string): Promise<Array<NetAddress>> {
      return new Promise<Array<NetAddress>>((resolve, reject) => {
        taskpool.execute((): Array<NetAddress> => {
            return this.getAddressesByNameSyncWithHandle(host);
        }).then((content: Any) => {
            resolve(content as Array<NetAddress>);
        }, (err: Error): void => {
            reject(err as BusinessError);
        });
      });
    }

    native getAddressesByNameWithOptionsSyncWithHandle(host: string, option?: QueryOptions): Array<NetAddress>;

    getAddressesByNameWithOptions(host: string, option?: QueryOptions): Promise<Array<NetAddress>> {
      return new Promise<Array<NetAddress>>((resolve, reject) => {
        let opt: QueryOptions = option ?? {};
        taskpool.execute((): Array<NetAddress> => {
            return this.getAddressesByNameWithOptionsSyncWithHandle(host, opt);
        }).then((content: Any) => {
            resolve(content as Array<NetAddress>);
        }).catch((err: Error) => {
            reject(toBusinessError(err));
        });
      });
    }
  }

  export interface NetCapabilities {
    linkUpBandwidthKbps?: int;

    linkDownBandwidthKbps?: int;

    networkCap?: Array<NetCap>;

    bearerTypes: Array<NetBearType>;
  }

  export class NetCapabilitiesInner implements NetCapabilities {
    linkUpBandwidthKbps?: int;

    linkDownBandwidthKbps?: int;

    networkCap?: Array<NetCap>;

    bearerTypes: Array<NetBearType>;
  }

  export interface NetConnectionPropertyInfo {
    netHandle: NetHandle;
    connectionProperties: ConnectionProperties;
  }

  export class NetConnectionPropertyInfoInner implements NetConnectionPropertyInfo {
    netHandle: NetHandle;
    connectionProperties: ConnectionProperties;
  }

  export interface NetBlockStatusInfo {
    netHandle: NetHandle;
    blocked: boolean;
  }

  export class NetBlockStatusInfoInner implements NetBlockStatusInfo {
    netHandle: NetHandle;
    blocked: boolean;
  }

  export enum NetCap {
    NET_CAPABILITY_MMS = 0,

    NET_CAPABILITY_NOT_METERED = 11,

    NET_CAPABILITY_INTERNET = 12,

    NET_CAPABILITY_NOT_VPN = 15,

    NET_CAPABILITY_VALIDATED = 16,
    NET_CAPABILITY_PORTAL = 17,

    NET_CAPABILITY_CHECKING_CONNECTIVITY = 31
  }

  export enum NetBearType {
    BEARER_CELLULAR = 0,

    BEARER_WIFI = 1,

    BEARER_BLUETOOTH = 2,

    BEARER_ETHERNET = 3,

    BEARER_VPN = 4,
  }

  export enum FamilyType {
    FAMILY_TYPE_ALL = 0,
    FAMILY_TYPE_IPV4 = 1,
    FAMILY_TYPE_IPV6 = 2,
  }

  export enum ConversionProcess {
    NO_CONFIGURATION = 0,
    ALLOW_UNASSIGNED = 1,
    USE_STD3_ASCII_RULES = 2,
  }

  export enum ProtocolType {
    PROTO_TYPE_TCP = 6,
    PROTO_TYPE_UDP = 17,
  }

  export enum TcpState {
    TCP_ESTABLISHED = 1,
    TCP_SYN_SENT = 2,
    TCP_SYN_RECV = 3,
    TCP_FIN_WAIT1 = 4,
    TCP_FIN_WAIT2 = 5,
    TCP_TIME_WAIT = 6,
    TCP_CLOSE = 7,
    TCP_CLOSE_WAIT = 8,
    TCP_LAST_ACK = 9,
    TCP_LISTEN = 10,
    TCP_CLOSING = 11,
  }

  export interface TcpNetPortStatesInfo {
    tcpLocalIp: string;
    tcpLocalPort: int;
    tcpRemoteIp: string;
    tcpRemotePort: int;
    tcpUid: int;
    tcpPid: int;
    tcpState: TcpState;
  }

  class TcpNetPortStatesInfoInner implements TcpNetPortStatesInfo {
    tcpLocalIp: string;
    tcpLocalPort: int;
    tcpRemoteIp: string;
    tcpRemotePort: int;
    tcpUid: int;
    tcpPid: int;
    tcpState: TcpState;
  }

  export interface UdpNetPortStatesInfo {
    udpLocalIp: string;
    udpLocalPort: int;
    udpUid: int;
    udpPid: int;
  }

  class UdpNetPortStatesInfoInner implements UdpNetPortStatesInfo {
    udpLocalIp: string;
    udpLocalPort: int;
    udpUid: int;
    udpPid: int;
  }

  export interface NetPortStatesInfo {
    tcpPortStatesInfo?: Array<TcpNetPortStatesInfo>;
    udpPortStatesInfo?: Array<UdpNetPortStatesInfo>;
  }

  class NetPortStatesInfoInner implements NetPortStatesInfo {
    tcpPortStatesInfo?: Array<TcpNetPortStatesInfo>;
    udpPortStatesInfo?: Array<UdpNetPortStatesInfo>;
  }

  export interface NetIpMacInfo {
    ipAddress: NetAddress;
    macAddress: string;
    iface: string;
  }

  class NetIpMacInfoInner implements NetIpMacInfo {
    ipAddress: NetAddress;
    macAddress: string;
    iface: string;
  }

  export enum PacketsType {
    NETCONN_PACKETS_ICMP = 0,
    NETCONN_PACKETS_UDP = 1,
  }

  export enum ProxyMode {
    PROXY_MODE_OFF = 0,
    PROXY_MODE_AUTO = 1,
  }

  export enum Socks5DnsStrategy {
    SYSTEM_MODE = 0,
    PROXY_MODE = 1,
  }

  export interface Socks5Proxy {
    host: string;
    port: int;
    username?: string;
    password?: string;
    dnsStrategy?: Socks5DnsStrategy;
    exclusionList?: Array<string>;
  }

  class Socks5ProxyInner implements Socks5Proxy {
    host: string;
    port: int;
    username?: string;
    password?: string;
    dnsStrategy?: Socks5DnsStrategy;
    exclusionList?: Array<string>;
  }

  export interface UDPSocket {
    socketFd: int;
  }

  class UDPSocketInner implements UDPSocket {
    socketFd: int;
  }

  export interface TCPSocket {
    socketFd: int;
  }

  class TCPSocketInner implements TCPSocket {
    socketFd: int;
  }

  export interface HttpRequest {
    url: string;
    method: string;
    headers: Array<string>;
    body?: string;
  }

  class HttpRequestInner implements HttpRequest {
    url: string;
    method: string;
    headers: Array<string>;
    body?: string;
  }

  export interface ProbeResultInfo {
    lossRate: int;
    rtt: Array<int>;
  }

  class ProbeResultInfoInner implements ProbeResultInfo {
    lossRate: int;
    rtt: Array<int>;
  }

  export interface TraceRouteInfo {
    jumpNo: int;
    address: string;
    rtt: Array<number>;
  }

  class TraceRouteInfoInner implements TraceRouteInfo {
    jumpNo: int;
    address: string;
    rtt: Array<number>;
  }

  export interface TraceRouteOptions {
    maxJumpNumber?: int;
    packetsType?: PacketsType;
  }

  class TraceRouteOptionsInner implements TraceRouteOptions {
    maxJumpNumber?: int;
    packetsType?: PacketsType;
  }

  export interface ConnectionProperties {
    interfaceName: string;
    domains: string;
    linkAddresses: Array<LinkAddress>;

    dnses: Array<NetAddress>;

    routes: Array<RouteInfo>;

    mtu: int;
  }

  class ConnectionPropertiesInner implements ConnectionProperties {
    interfaceName: string;
    domains: string;
    linkAddresses: Array<LinkAddress>;

    dnses: Array<NetAddress>;

    routes: Array<RouteInfo>;

    mtu: int;
  }

  export interface RouteInfo {
    iface: string;

    destination: LinkAddress;

    gateway: NetAddress;

    hasGateway: boolean;

    isDefaultRoute: boolean;
  }

  class RouteInfoInner implements RouteInfo {
    iface: string;

    destination: LinkAddress = new LinkAddressInner;

    gateway: NetAddress = new NetAddressInner;

    hasGateway: boolean;

    isDefaultRoute: boolean;
  }

  export interface LinkAddress {
    address: NetAddress;
    prefixLength: int;
  }

  class LinkAddressInner implements LinkAddress {
    address: NetAddress = new NetAddressInner;
    prefixLength: int;
  }

  export interface NetAddress {
    address: string;

    family?: int;

    port?: int;
  }

  class NetAddressInner implements NetAddress {
    address: string;

    family?: int;

    port?: int;
  }

  export interface QueryOptions {
    family?: FamilyType;
  }

  class QueryOptionsInner implements QueryOptions {
    family?: FamilyType;
  }

  export interface NetConnInfoParam {
    protocolType: ProtocolType;
    family: FamilyType;
    localAddress: string;
    localPort: int;
    remoteAddress: string;
    remotePort: int;
  }

  export class NetConnInfoParamInner implements NetConnInfoParam {
    protocolType: ProtocolType;
    family: FamilyType;
    localAddress: string;
    localPort: int;
    remoteAddress: string;
    remotePort: int;

    constructor(protocolType: ProtocolType, family: FamilyType, localAddress: string,
      localPort: int, remoteAddress: string, remotePort: int) {
      this.protocolType = protocolType;
      this.family = family;
      this.localAddress = localAddress;
      this.localPort = localPort;
      this.remoteAddress = remoteAddress;
      this.remotePort = remotePort;
    }
  }

  export interface HttpProxy {
    host: string;

    port: int;

    username?: string;

    password?: string;

    exclusionList: Array<string>;
  }

  export class HttpProxyInner implements HttpProxy {
    host: string;

    port: int;

    username?: string;

    password?: string;

    exclusionList: Array<string>;
  }
}
