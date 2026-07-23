/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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

import { AsyncCallback, Callback } from './@ohos.base';
import http from './@ohos.net.http';
import socket from './@ohos.net.socket';

/**
 * Provides interfaces to manage and use data networks.
 * @namespace connection
 * @syscap SystemCapability.Communication.NetManager.Core
 * @since 8
 */
/**
 * Provides interfaces to manage and use data networks.
 * @namespace connection
 * @syscap SystemCapability.Communication.NetManager.Core
 * @crossplatform
 * @since 10
 */
declare namespace connection {
  type HttpRequest = http.HttpRequest;
  type TCPSocket = socket.TCPSocket;
  type UDPSocket = socket.UDPSocket;

  /**
   * Create a network connection with optional netSpecifier and timeout.
   * @param { NetSpecifier } netSpecifier Indicates the network specifier. See {@link NetSpecifier}.
   * @param { number } timeout The time in milliseconds to attempt looking for a suitable network before
   * {@link NetConnection#netUnavailable} is called.
   * @returns { NetConnection } the NetConnection of the NetSpecifier.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  /**
   * Create a network connection with optional netSpecifier and timeout.
   * @param { NetSpecifier } netSpecifier Indicates the network specifier. See {@link NetSpecifier}.
   * @param { number } timeout The time in milliseconds to attempt looking for a suitable network before
   * {@link NetConnection#netUnavailable} is called.
   * @returns { NetConnection } the NetConnection of the NetSpecifier.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @crossplatform
   * @since 10
   */
  function createNetConnection(netSpecifier?: NetSpecifier, timeout?: number): NetConnection;

  /**
   * Create a network interface object.
   * @returns { NetInterface } the NetInterface object.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  function createNetInterface(): NetInterface;

  /**
   * Add route for a specefic net.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { number } netId - the nethandle to add route.
   * @param { RouteInfo } routeInfo - the route info to add.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  function addNetworkRoute(netId: number, routeInfo: RouteInfo): void;

  /**
   * Get the config of the interface.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { string } ifaceName - the name of the interface.
   * @param { AsyncCallback<NetInterfaceConfiguration> } callback - the callback of getNetInterfaceConfiguration.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  function getNetInterfaceConfiguration(ifaceName: string, callback: AsyncCallback<NetInterfaceConfiguration>): void;

    /**
   * Get the config of the interface.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { string } ifaceName - the name of the interface.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  function getNetInterfaceConfiguration(ifaceName: string): Promise<NetInterfaceConfiguration>;

  /**
   * Register a net supplier.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { NetBearType } bearerType - the bearer type of the net.
   * @param { string } ident - the id of the net.
   * @param { Array<NetCap> } netCaps - the net capabilities of the net.
   * @param { AsyncCallback<number> } callback - the callback of registerNetSupplier.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  function registerNetSupplier(bearerType: NetBearType, ident: string, netCaps: Array<NetCap>, callback: AsyncCallback<number>): void;

  /**
   * Get the config of the interface.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { string } ifaceName - the name of the interface.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  function registerNetSupplier(bearerType: NetBearType, ident: string, netCaps: Array<NetCap>): Promise<number>;

  /**
   * Unregister a net supplier.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { number } netId - the id of the net.
   * @param { AsyncCallback<void> } callback - the callback of unregisterNetSupplier.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  function unregisterNetSupplier(netId: number, callback: AsyncCallback<void>): void;

  /**
   * Unregister a net supplier.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { number } netId - the id of the net.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  function unregisterNetSupplier(netId: number): Promise<void>;

  /**
   * Obtains the data network that is activated by default.
   * To call this method, you must have the {@code ohos.permission.GET_NETWORK_INFO} permission.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @param { AsyncCallback<NetHandle> } callback Returns the {@link NetHandle} object;
   * returns {@code null} if the default network is not activated.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function getDefaultNet(callback: AsyncCallback<NetHandle>): void;

  /**
   * Obtains the data network that is activated by default.
   * To call this method, you must have the {@code ohos.permission.GET_NETWORK_INFO} permission.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @returns { Promise<NetHandle> } The promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function getDefaultNet(): Promise<NetHandle>;

  /**
   * Obtains the data network that is activated by default.
   * To call this method, you must have the {@code ohos.permission.GET_NETWORK_INFO} permission.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @returns { NetHandle } if the default network is not activated.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 9
   */
  function getDefaultNetSync(): NetHandle;

  /**
   * Obtains the list of data networks that are activated.
   * To invoke this method, you must have the {@code ohos.permission.GET_NETWORK_INFO} permission.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @param { AsyncCallback<Array<NetHandle>> } callback Returns the {@link NetHandle} object; returns {@code null} if no network is activated.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function getAllNets(callback: AsyncCallback<Array<NetHandle>>): void;

  /**
   * Obtains the list of data networks that are activated.
   * To invoke this method, you must have the {@code ohos.permission.GET_NETWORK_INFO} permission.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @returns { Promise<Array<NetHandle>> } The promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function getAllNets(): Promise<Array<NetHandle>>;

  /**
   * Queries the connection properties of a network.
   * This method requires the {@code ohos.permission.GET_NETWORK_INFO} permission.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @param { NetHandle } netHandle Indicates the network to be queried.
   * @param { AsyncCallback<ConnectionProperties> } callback Returns the {@link ConnectionProperties} object.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function getConnectionProperties(netHandle: NetHandle, callback: AsyncCallback<ConnectionProperties>): void;

  /**
   * Queries the connection properties of a network.
   * This method requires the {@code ohos.permission.GET_NETWORK_INFO} permission.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @param { NetHandle } netHandle Indicates the network to be queried.
   * @returns { Promise<ConnectionProperties> } The promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function getConnectionProperties(netHandle: NetHandle): Promise<ConnectionProperties>;

  /**
   * Obtains {@link NetCapabilities} of a {@link NetHandle} object.
   * To invoke this method, you must have the {@code ohos.permission.GET_NETWORK_INFO} permission.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @param { NetHandle } netHandle Indicates the handle. See {@link NetHandle}.
   * @param { AsyncCallback<NetCapabilities> } callback Returns {@link NetCapabilities}; returns {@code null} if {@code handle} is invalid.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function getNetCapabilities(netHandle: NetHandle, callback: AsyncCallback<NetCapabilities>): void;

  /**
   * Obtains {@link NetCapabilities} of a {@link NetHandle} object.
   * To invoke this method, you must have the {@code ohos.permission.GET_NETWORK_INFO} permission.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @param { NetHandle } netHandle Indicates the handle. See {@link NetHandle}.
   * @returns { Promise<NetCapabilities> } The promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function getNetCapabilities(netHandle: NetHandle): Promise<NetCapabilities>;

  /**
   * Checks whether data traffic usage on the current network is metered.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @param { AsyncCallback<boolean> } callback Returns {@code true} if data traffic usage on the current network is metered;
   * returns {@code false} otherwise.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 9
   */
  function isDefaultNetMetered(callback: AsyncCallback<boolean>): void;

  /**
   * Checks whether data traffic usage on the current network is metered.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @returns { Promise<boolean> } the promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 9
   */
  function isDefaultNetMetered(): Promise<boolean>;

  /**
   * Checks whether the default data network is activated.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @param { AsyncCallback<boolean> } callback Returns {@code true} if the default data network is activated;
   * returns {@code false} otherwise.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  /**
   * Checks whether the default data network is activated.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @param { AsyncCallback<boolean> } callback Returns {@code true} if the default data network is activated;
   * returns {@code false} otherwise.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @crossplatform
   * @since 10
   */
  function hasDefaultNet(callback: AsyncCallback<boolean>): void;

  /**
   * Checks whether the default data network is activated.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @returns { Promise<boolean> } The promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  /**
   * Checks whether the default data network is activated.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @returns { Promise<boolean> } The promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @crossplatform
   * @since 10
   */
  function hasDefaultNet(): Promise<boolean>;

  /**
   * Enables the airplane mode for a device.
   * To invoke this method, you must have the {@code ohos.permission.CONNECTIVITY_INTERNAL} permission.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { AsyncCallback<void> } callback - the callback of enableAirplaneMode.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 8
   */
  function enableAirplaneMode(callback: AsyncCallback<void>): void;

  /**
   * Enables the airplane mode for a device.
   * To invoke this method, you must have the {@code ohos.permission.CONNECTIVITY_INTERNAL} permission.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @returns { Promise<void> } The promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 8
   */
  function enableAirplaneMode(): Promise<void>;

  /**
   * Disables the airplane mode for a device.
   * To invoke this method, you must have the {@code ohos.permission.CONNECTIVITY_INTERNAL} permission.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { AsyncCallback<void> } callback - the callback of disableAirplaneMode.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 8
   */
  function disableAirplaneMode(callback: AsyncCallback<void>): void;

  /**
   * Disables the airplane mode for a device.
   * To invoke this method, you must have the {@code ohos.permission.CONNECTIVITY_INTERNAL} permission.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @returns { Promise<void> } The promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 8
   */
  function disableAirplaneMode(): Promise<void>;

  /**
   * Reports the network state is connected.
   * @permission ohos.permission.GET_NETWORK_INFO and ohos.permission.INTERNET
   * @param { NetHandle } netHandle Indicates the network whose state is to be reported.
   * @param { AsyncCallback<void> } callback - the callback of reportNetConnected.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function reportNetConnected(netHandle: NetHandle, callback: AsyncCallback<void>): void;

  /**
   * Reports the network state is connected.
   * @permission ohos.permission.GET_NETWORK_INFO and ohos.permission.INTERNET
   * @param { NetHandle } netHandle Indicates the network whose state is to be reported.
   * @returns { Promise<void> } The promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function reportNetConnected(netHandle: NetHandle): Promise<void>;

  /**
   * Reports the network state is disconnected.
   * @permission ohos.permission.GET_NETWORK_INFO and ohos.permission.INTERNET
   * @param { NetHandle } netHandle Indicates the network whose state is to be reported.
   * @param { AsyncCallback<void> } callback - the callback of reportNetDisconnected.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function reportNetDisconnected(netHandle: NetHandle, callback: AsyncCallback<void>): void;

  /**
   * Reports the network state is disconnected.
   * @permission ohos.permission.GET_NETWORK_INFO and ohos.permission.INTERNET
   * @param { NetHandle } netHandle Indicates the network whose state is to be reported.
   * @returns { Promise<void> } The promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function reportNetDisconnected(netHandle: NetHandle): Promise<void>;

  /**
   * Resolves the host name to obtain all IP addresses based on the default data network.
   * @permission ohos.permission.INTERNET
   * @param { string } host Indicates the host name or the domain.
   * @param { AsyncCallback<Array<NetAddress>> } callback Returns the NetAddress list.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function getAddressesByName(host: string, callback: AsyncCallback<Array<NetAddress>>): void;

  /**
   * Resolves the host name to obtain all IP addresses based on the default data network.
   * @permission ohos.permission.INTERNET
   * @param { string } host Indicates the host name or the domain.
   * @returns { Promise<Array<NetAddress>> } The promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  function getAddressesByName(host: string): Promise<Array<NetAddress>>;


  /**
   * Resolves a host name to obtain all IP addresses based on the specified NetHandle.
   * @permission ohos.permission.INTERNET
   * @param { string } host Indicates the host name or the domain.
   * @param { QueryOptions } option - Indicates the query option.
   * @returns { Promise<Array<NetAddress>> } The promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Failed to connect to the service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 23 dynamic&static
   */
  function getAddressesByNameWithOptions(host: string, option?: QueryOptions): Promise<Array<NetAddress>>;

  /**
   * Obtains the {@link NetHandle} bound to a process using {@link setAppNet}.
   * @param { AsyncCallback<NetHandle> } callback Returns the {@link NetHandle} bound to the process;
   * returns {@code null} if no {@link NetHandle} is bound to the process.For details, see {@link NetHandle}.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 9
   */
  function getAppNet(callback: AsyncCallback<NetHandle>): void;

  /**
   * Obtains the {@link NetHandle} bound to a process using {@link setAppNet}.
   * @returns { Promise<NetHandle> } the promise returned by the function.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 9
   */
  function getAppNet(): Promise<NetHandle>;

  /**
   * Binds a process to {@code NetHandle}.
   * <p>All the sockets created from the process will be bound to the {@code NetHandle},
   * and the resolution of all host names will be managed by the {@code NetHandle}.</p>
   * @permission ohos.permission.INTERNET
   * @param { NetHandle } netHandle Indicates the handle. For details, see {@link NetHandle}.
   * @param { AsyncCallback<void> } callback Returns the callback of setAppNet.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 9
   */
  function setAppNet(netHandle: NetHandle, callback: AsyncCallback<void>): void;

  /**
   * Binds a process to {@code NetHandle}.
   * <p>All the sockets created from the process will be bound to the {@code NetHandle},
   * and the resolution of all host names will be managed by the {@code NetHandle}.</p>
   * @permission ohos.permission.INTERNET
   * @param { NetHandle } netHandle Indicates the handle. For details, see {@link NetHandle}.
   * @returns { Promise<void> } the promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 9
   */
  function setAppNet(netHandle: NetHandle): Promise<void>;

  /**
   * Set a specific interface up.
   * To invoke this method, you must have the {@code ohos.permission.CONNECTIVITY_INTERNAL} permission.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { string } ifaceName - the name of the interface to set up.
   * @param { AsyncCallback<void> } callback - the callback of setInterfaceUp.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  function setInterfaceUp(ifaceName: string, callback: AsyncCallback<void>): void;

  /**
   * Set a specific interface up.
   * @param { string } ifaceName - the name of the interface to set up.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  function setInterfaceUp(ifaceName: string): Promise<void>;

  /**
   * Set ip address for a specific interface.
   * To invoke this method, you must have the {@code ohos.permission.CONNECTIVITY_INTERNAL} permission.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { string } ifaceName - the name of the interface.
   * @param { string } ip - the ip address to set for the interface.
   * @param { AsyncCallback<void> } callback - the callback of setNetInterfaceIpAddress.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  function setNetInterfaceIpAddress(ifaceName: string, ip: string, callback: AsyncCallback<void>): void;

  /**
   * Set ip address for a specific interface.
   * @param { string } ifaceName - the name of the interface.
   * @param { string } ip - the ip address to set for the interface.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  function setNetInterfaceIpAddress(ifaceName: string, ip: string): Promise<void>;

  /**
   * Obtains the network independent global {@link HttpProxy} proxy settings.
   *
   * If a application level proxy is set, the application level proxy parameters are returned.
   * If a global proxy is set, the global proxy parameters are returned.
   * If the process is bound to a {@link NetHandle} using {@link setAppNet}, the {@link NetHandle} proxy settings are returned.
   * In other cases, the proxy settings of default network are returned.
   *
   * @param { AsyncCallback<HttpProxy> } callback Returns the proxy settings. For details, see {@link HttpProxy}.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use.
   * @since 10
   */
  function getGlobalHttpProxy(callback: AsyncCallback<HttpProxy>): void;

  /**
   * Obtains the network independent global {@link HttpProxy} proxy settings.
   *
   * If a application level proxy is set, the application level proxy parameters are returned.
   * If a global proxy is set, the global proxy parameters are returned.
   * If the process is bound to a {@link NetHandle} using {@link setAppNet}, the {@link NetHandle} proxy settings are returned.
   * In other cases, the proxy settings of default network are returned.
   *
   * @returns { Promise<HttpProxy> } the promise returned by the function.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use.
   * @since 10
   */
  function getGlobalHttpProxy(): Promise<HttpProxy>;

  /**
   * Set application level http proxy {@link HttpProxy}.
   * @param { HttpProxy } httpProxy - Indicates the application level proxy settings. For details, see {@link HttpProxy}.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid http proxy.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 11
   */

  function setAppHttpProxy(httpProxy: HttpProxy): void;

  /**
   * Set a network independent global {@link HttpProxy} proxy settings.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { HttpProxy } httpProxy Indicates the global proxy settings. For details, see {@link HttpProxy}.
   * @param { AsyncCallback<void> } callback Returns the callback of setGlobalHttpProxy.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use.
   * @since 10
   */
  function setGlobalHttpProxy(httpProxy: HttpProxy, callback: AsyncCallback<void>): void;

  /**
   * Set a network independent global {@link HttpProxy} proxy settings.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @param { HttpProxy } httpProxy Indicates the global proxy settings. For details, see {@link HttpProxy}.
   * @returns { Promise<void> } the promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use.
   * @since 10
   */
  function setGlobalHttpProxy(httpProxy: HttpProxy): Promise<void>;

  /**
   * Notifies the system that global proxy re-authentication is required.
   * Upon receiving the notification, the system will reproces the global proxy's authentication status.
   *
   * @returns { Promise<HttpProxy> } the promise returned by the function.
   * @throws { BusinessError } 2100002 - Failed to connect to the service..
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @stagemodelonly
   * @since 26.0.0 dynamic&static
   */
  function refreshGlobalHttpProxy(): Promise<HttpProxy>;

  /**
   * Obtains the default {@link HttpProxy} proxy settings.
   *
   * If a global proxy is set, the global proxy parameters are returned.
   * If the process is bound to a {@link NetHandle} using {@link setAppNet},
   * the {@link NetHandle} proxy settings are returned.
   * In other cases, the proxy settings of default network are returned.
   *
   * @param { AsyncCallback<HttpProxy> } callback Returns the default {@link HttpProxy} settings.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 10
   */
  function getDefaultHttpProxy(callback: AsyncCallback<HttpProxy>): void;

  /**
   * Obtains the default {@link HttpProxy} proxy settings.
   *
   * If a global proxy is set, the global proxy parameters are returned.
   * If the process is bound to a {@link NetHandle} using {@link setAppNet},
   * the {@link NetHandle} proxy settings are returned.
   * In other cases, the proxy settings of default network are returned.
   *
   * @returns { Promise<HttpProxy> } the promise returned by the function.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 10
   */
  function getDefaultHttpProxy(): Promise<HttpProxy>;

  /**
   * Add a custom {@link host} and corresponding {@link ip} mapping.
   * @permission ohos.permission.INTERNET
   * @param { string } host - Indicates the host name or the domain.
   * @param { Array<string> } ip - List of IP addresses mapped to the host name.
   * @param { AsyncCallback<void> } callback - Returns the callback of addCustomDnsRule.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 11
   */
  function addCustomDnsRule(host: string, ip: Array<string>, callback: AsyncCallback<void>): void;

  /**
   * Add a custom {@link host} and corresponding {@link ip} mapping.
   * @permission ohos.permission.INTERNET
   * @param { string } host - Indicates the host name or the domain.
   * @param { Array<string> } ip - List of IP addresses mapped to the host name.
   * @returns { Promise<void> } the promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 11
   */
  function addCustomDnsRule(host: string, ip: Array<string>): Promise<void>;

  /**
   * Remove the custom DNS rule of the {@link host}.
   * @permission ohos.permission.INTERNET
   * @param { string } host - Indicates the host name or the domain.
   * @param { AsyncCallback<void> } callback - Returns the callback of removeCustomDnsRule.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 11
   */
  function removeCustomDnsRule(host: string, callback: AsyncCallback<void>): void;

  /**
   * Remove the custom DNS rule of the {@link host}.
   * @permission ohos.permission.INTERNET
   * @param { string } host - Indicates the host name or the domain.
   * @returns { Promise<void> } the promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 11
   */
  function removeCustomDnsRule(host: string): Promise<void>;

  /**
   * Clear all custom DNS rules.
   * @permission ohos.permission.INTERNET
   * @param { AsyncCallback<void> } callback - Returns the callback of clearCustomDnsRules.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 11
   */
  function clearCustomDnsRules(callback: AsyncCallback<void>): void;

  /**
   * Clear all custom DNS rules.
   * @permission ohos.permission.INTERNET
   * @returns { Promise<void> } the promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100001 - Invalid parameter value.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 11
   */
  function clearCustomDnsRules(): Promise<void>;

  /**
   * factory reset network settings
   *
   * To invoke this method, you must have the {@code ohos.permission.CONNECTIVITY_INTERNAL} permission.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @returns { Promise<void> } the promise returned by the function.
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Non-system applications use system APIs.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
   * @throws { BusinessError } 2100003 - System internal error.
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 11
   */
  function factoryReset(): Promise<void>;

  /**
   * Represents the network connection handle.
   * @interface NetConnection
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  /**
   * Represents the network connection handle.
   * @interface NetConnection
   * @syscap SystemCapability.Communication.NetManager.Core
   * @crossplatform
   * @since 10
   */
  export interface NetConnection {
    /**
     * Registers a listener for netAvailable events.
     * @param { 'netAvailable' } type Indicates Event name.
     * @param { Callback<NetHandle> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    /**
     * Registers a listener for netAvailable events.
     * @param { 'netAvailable' } type Indicates Event name.
     * @param { Callback<NetHandle> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @crossplatform
     * @since 10
     */
    on(type: 'netAvailable', callback: Callback<NetHandle>): void;

    /**
     * Registers a listener for netBlockStatusChange events.
     * @param { 'netBlockStatusChange' } type Indicates Event name.
     * @param { Callback<{ netHandle: NetHandle, blocked: boolean }> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    on(type: 'netBlockStatusChange', callback: Callback<{ netHandle: NetHandle, blocked: boolean }>): void;

    /**
     * Registers a listener for **netCapabilitiesChange** events.
     * @param { 'netCapabilitiesChange' } type Indicates Event name.
     * @param { Callback<{ netHandle: NetHandle, netCap: NetCapabilities }> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    /**
     * Registers a listener for **netCapabilitiesChange** events.
     * @param { 'netCapabilitiesChange' } type Indicates Event name.
     * @param { Callback<{ netHandle: NetHandle, netCap: NetCapabilities }> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @crossplatform
     * @since 10
     */
    on(type: 'netCapabilitiesChange', callback: Callback<{ netHandle: NetHandle, netCap: NetCapabilities }>): void;

    /**
     * Registers a listener for netConnectionPropertiesChange events.
     * @param { 'netConnectionPropertiesChange' } type Indicates Event name.
     * @param { Callback<{ netHandle: NetHandle, connectionProperties: ConnectionProperties }> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    on(type: 'netConnectionPropertiesChange', callback: Callback<{ netHandle: NetHandle, connectionProperties: ConnectionProperties }>): void;

    /**
     * Registers a listener for **netLost** events.
     * @param { 'netLost' } type Indicates Event name.
     * @param { Callback<NetHandle> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    /**
     * Registers a listener for **netLost** events.
     * @param { 'netLost' } type Indicates Event name.
     * @param { Callback<NetHandle> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @crossplatform
     * @since 10
     */
    on(type: 'netLost', callback: Callback<NetHandle>): void;

    /**
     * Registers a listener for netUnavailable events.
     * @param { 'netUnavailable' } type Indicates Event name.
     * @param { Callback<void> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    /**
     * Registers a listener for netUnavailable events.
     * @param { 'netUnavailable' } type Indicates Event name.
     * @param { Callback<void> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @crossplatform
     * @since 10
     */
    on(type: 'netUnavailable', callback: Callback<void>): void;

    /**
     * Receives status change notifications of a specified network.
     * @permission ohos.permission.GET_NETWORK_INFO
     * @param { AsyncCallback<void> } callback - the callback of register.
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 401 - Parameter error.
     * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @throws { BusinessError } 2101008 - The same callback exists.
     * @throws { BusinessError } 2101022 - The number of requests exceeded the maximum.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    /**
     * Receives status change notifications of a specified network.
     * @permission ohos.permission.GET_NETWORK_INFO
     * @param { AsyncCallback<void> } callback - the callback of register.
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 401 - Parameter error.
     * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @throws { BusinessError } 2101008 - The same callback exists.
     * @throws { BusinessError } 2101022 - The number of requests exceeded the maximum.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @crossplatform
     * @since 10
     */
    register(callback: AsyncCallback<void>): void;

    /**
     * Cancels listening for network status changes.
     * @param { AsyncCallback<void> } callback - the callback of unregister.
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 401 - Parameter error.
     * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @throws { BusinessError } 2101007 - The callback is not exists.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    /**
     * Cancels listening for network status changes.
     * @param { AsyncCallback<void> } callback - the callback of unregister.
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 401 - Parameter error.
     * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @throws { BusinessError } 2101007 - The callback is not exists.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @crossplatform
     * @since 10
     */
    unregister(callback: AsyncCallback<void>): void;
  }

  /**
   * Represents the network interface.
   * @interface NetInterface
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use. Only used for system app.
   * @since 16
   */
  export interface NetInterface {
    /**
     * Registers a listener for interfaceAddressUpdated events.
     * @param { 'interfaceAddressUpdated' } type Indicates Event name.
     * @param { Callback<InterfaceInfo> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use. Only used for system app.
     * @since 16
     */
    on(type: 'interfaceAddressUpdated', callback: Callback<InterfaceInfo>): void;
    /**
     * Registers a listener for interfaceAddressRemoved events.
     * @param { 'interfaceAddressRemoved' } type Indicates Event name.
     * @param { Callback<InterfaceInfo> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use. Only used for system app.
     * @since 16
     */
    on(type: 'interfaceAddressRemoved', callback: Callback<InterfaceInfo>): void;
    /**
     * Registers a listener for interfaceAdded events.
     * @param { 'interfaceAdded' } type Indicates Event name.
     * @param { Callback<InterfaceInfo> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use. Only used for system app.
     * @since 16
     */
    on(type: 'interfaceAdded', callback: Callback<InterfaceInfo>): void;
    /**
     * Registers a listener for interfaceRemoved events.
     * @param { 'interfaceRemoved' } type Indicates Event name.
     * @param { Callback<InterfaceInfo> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use. Only used for system app.
     * @since 16
     */
    on(type: 'interfaceRemoved', callback: Callback<InterfaceInfo>): void;
    /**
     * Registers a listener for interfaceChanged events.
     * @param { 'interfaceChanged' } type Indicates Event name.
     * @param { Callback<InterfaceInfo> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use. Only used for system app.
     * @since 16
     */
    on(type: 'interfaceChanged', callback: Callback<InterfaceInfo>): void;
    /**
     * Registers a listener for interfaceLinkStateChanged events.
     * @param { 'interfaceLinkStateChanged' } type Indicates Event name.
     * @param { Callback<InterfaceInfo> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use. Only used for system app.
     * @since 16
     */
    on(type: 'interfaceLinkStateChanged', callback: Callback<InterfaceInfo>): void;
    /**
     * Registers a listener for routeChanged events.
     * @param { 'routeChanged' } type Indicates Event name.
     * @param { Callback<RouteChangeInfo> } callback - the callback of on.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use. Only used for system app.
     * @since 16
     */
    on(type: 'routeChanged', callback: Callback<RouteChangeInfo>): void;

    /**
     * Receives status change notifications of a specified interface.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @param { AsyncCallback<void> } callback - the callback of register.
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 401 - Parameter error.
     * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @throws { BusinessError } 2101008 - The same callback exists.
     * @throws { BusinessError } 2101022 - The number of requests exceeded the maximum.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use. Only used for system app.
     * @since 16
     */
    register(callback: AsyncCallback<void>): void;
    /**
     * Cancels listening for interface status changes.
     * @param { AsyncCallback<void> } callback - the callback of unregister.
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 401 - Parameter error.
     * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @throws { BusinessError } 2101007 - The callback is not exists.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use. Only used for system app.
     * @since 16
     */
    unregister(callback: AsyncCallback<void>): void;
  }

  /**
   * Provides an instance that bear data network capabilities.
   * @interface NetSpecifier
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  export interface NetSpecifier {
    /**
     * The transmission capacity and support of the network's global proxy storage data network.
     * @type {NetCapabilities}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    netCapabilities: NetCapabilities;

    /**
     * Network identifier, the identifier for Wi Fi networks is "wifi", and the identifier for cellular networks is "simId1" (corresponding to SIM card 1).
     * @type {?string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    bearerPrivateIdentifier?: string;
  }

  /**
   * Defines the handle of the data network.
   * @interface NetHandle
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  /**
   * Defines the handle of the data network.
   * @interface NetHandle
   * @syscap SystemCapability.Communication.NetManager.Core
   * @crossplatform
   * @since 10
   */
  export interface NetHandle {
    /**
     * Network ID, a value of 0 means that there is no default network, and the other values must be greater than or equal to 100.
     * @type {number}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    /**
     * Network ID, a value of 0 means that there is no default network, and the other values must be greater than or equal to 100.
     * @type {number}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @crossplatform
     * @since 10
     */
    netId: number;

    /**
     * <p>Binds a TCPSocket or UDPSocket to the current network. All data flows from
     * the socket will use this network, without being subject to {@link setAppNet}.</p>
     * Before using this method, ensure that the socket is disconnected.
     * @param { TCPSocket | UDPSocket } socketParam Indicates the TCPSocket or UDPSocket object.
     * @param { AsyncCallback<void> } callback - the callback of bindSocket.
     * @throws { BusinessError } 401 - Parameter error.
     * @throws { BusinessError } 2100001 - Invalid parameter value.
     * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 9
     */
    bindSocket(socketParam: TCPSocket | UDPSocket, callback: AsyncCallback<void>): void;

    /**
     * <p>Binds a TCPSocket or UDPSocket to the current network. All data flows from
     * the socket will use this network, without being subject to {@link setAppNet}.</p>
     * Before using this method, ensure that the socket is disconnected.
     * @param { TCPSocket | UDPSocket } socketParam Indicates the TCPSocket or UDPSocket object.
     * @returns { Promise<void> } the promise returned by the function.
     * @throws { BusinessError } 401 - Parameter error.
     * @throws { BusinessError } 2100001 - Invalid parameter value.
     * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 9
     */
    bindSocket(socketParam: TCPSocket | UDPSocket): Promise<void>;

    /**
     * Resolves a host name to obtain all IP addresses based on the specified NetHandle.
     * @permission ohos.permission.INTERNET
     * @param { string } host Indicates the host name or the domain.
     * @param { AsyncCallback<Array<NetAddress>> } callback Returns the NetAddress list.
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 401 - Parameter error.
     * @throws { BusinessError } 2100001 - Invalid parameter value.
     * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    getAddressesByName(host: string, callback: AsyncCallback<Array<NetAddress>>): void;

    /**
     * Resolves a host name to obtain all IP addresses based on the specified NetHandle.
     * @permission ohos.permission.INTERNET
     * @param { string } host Indicates the host name or the domain.
     * @returns { Promise<Array<NetAddress>> } The promise returned by the function.
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 401 - Parameter error.
     * @throws { BusinessError } 2100001 - Invalid parameter value.
     * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    getAddressesByName(host: string): Promise<Array<NetAddress>>;
    
    /**
     * Resolves a host name to obtain all IP addresses based on the specified NetHandle.
     * @permission ohos.permission.INTERNET
     * @param { string } host Indicates the host name or the domain.
     * @param { QueryOptions } option - Indicates the query option.
     * @returns { Promise<Array<NetAddress>> } The promise returned by the function.
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 2100001 - Invalid parameter value.
     * @throws { BusinessError } 2100002 - Failed to connect to the service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 23 dynamic&static
     */
    getAddressesByNameWithOptions(host: string, option?: QueryOptions): Promise<Array<NetAddress>>;

    /**
     * Resolves a host name to obtain the first IP address based on the specified NetHandle.
     * @permission ohos.permission.INTERNET
     * @param { string } host Indicates the host name or the domain.
     * @param { AsyncCallback<NetAddress> } callback Returns the first NetAddress.
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 401 - Parameter error.
     * @throws { BusinessError } 2100001 - Invalid parameter value.
     * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    getAddressByName(host: string, callback: AsyncCallback<NetAddress>): void;

    /**
     * Resolves a host name to obtain the first IP address based on the specified NetHandle.
     * @permission ohos.permission.INTERNET
     * @param { string } host Indicates the host name or the domain.
     * @returns { Promise<NetAddress> } The promise returned by the function.
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 401 - Parameter error.
     * @throws { BusinessError } 2100001 - Invalid parameter value.
     * @throws { BusinessError } 2100002 - Operation failed. Cannot connect to service.
     * @throws { BusinessError } 2100003 - System internal error.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    getAddressByName(host: string): Promise<NetAddress>;
  }
  
  /**
   * Defines option of DNS query.
   * @interface QueryOptions
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 23 dynamic&static
   */
  export interface QueryOptions {
    family?: FamilyType;
  }

  /**
   * Defines address family type.
   * @enum {int}
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 23 dynamic&static
   */
  export enum FamilyType {
    /**
     * Indicates that no ip type is specified, all address types can be use.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 23 dynamic&static
     */
    FAMILY_TYPE_ALL = 0,
    /**
     * Indicates that family type is ipv4.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 23 dynamic&static
     */
    FAMILY_TYPE_IPV4 = 1,
    /**
     * Indicates that family type is ipv6.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 23 dynamic&static
     */
    FAMILY_TYPE_IPV6 = 2,
  }

  /**
   * Defines the network capability set.
   * @interface NetCapabilities
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  /**
   * Defines the network capability set.
   * @interface NetCapabilities
   * @syscap SystemCapability.Communication.NetManager.Core
   * @crossplatform
   * @since 10
   */
  export interface NetCapabilities {
    /**
     * Uplink (device-to-network) bandwidth.
     * @type {?number}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    linkUpBandwidthKbps?: number;

    /**
     * Downstream (network-to-device) bandwidth.
     * @type {?number}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    linkDownBandwidthKbps?: number;

    /**
     * Network-specific capabilities.
     * @type {?Array<NetCap>}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    networkCap?: Array<NetCap>;

    /**
     * Network type.
     * @type {Array<NetBearType>}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    /**
     * Network type.
     * @type {Array<NetBearType>}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @crossplatform
     * @since 10
     */
    bearerTypes: Array<NetBearType>;
  }

  /**
   * Defines the network capability.
   * @enum {number}
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  export enum NetCap {
    /**
     * Indicates that the network can access the carrier's MMSC to send and receive multimedia messages.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    NET_CAPABILITY_MMS = 0,

    /**
     * Indicates that the network traffic is not metered.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    NET_CAPABILITY_NOT_METERED = 11,

    /**
     * Indicates that the network can access the Internet.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    NET_CAPABILITY_INTERNET = 12,

    /**
     * Indicates that the network does not use a VPN.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    NET_CAPABILITY_NOT_VPN = 15,

    /**
     * Indicates that the network is available.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    NET_CAPABILITY_VALIDATED = 16,
    /**
     * Indicates oem paid.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    NET_CAPABILITY_OEM_PAID = 26,
    /**
     * Indicates oem private.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    NET_CAPABILITY_OEM_PRIVATE = 27,
  }

  /**
   * Enumerates network types.
   * @enum {number}
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  /**
   * Enumerates network types.
   * @enum {number}
   * @syscap SystemCapability.Communication.NetManager.Core
   * @crossplatform
   * @since 10
   */
  export enum NetBearType {
    /**
     * Indicates that the network is based on a cellular network.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    /**
     * Indicates that the network is based on a cellular network.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @crossplatform
     * @since 10
     */
    BEARER_CELLULAR = 0,

    /**
     * Indicates that the network is based on a Wi-Fi network.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    /**
     * Indicates that the network is based on a Wi-Fi network.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @crossplatform
     * @since 10
     */
    BEARER_WIFI = 1,

    /**
     * Indicates that the network is an Ethernet network.
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    BEARER_ETHERNET = 3,
  }

  /**
   * Defines the network connection properties.
   * @interface ConnectionProperties
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  export interface ConnectionProperties {
    /**
     * Network card name.
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    interfaceName: string;
    /**
     * Domain. The default value is "".
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    domains: string;
    /**
     * Link information.
     * @type {Array<LinkAddress>}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    linkAddresses: Array<LinkAddress>;

    /**
     * Network address, refer to [NetAddress].
     * @type {Array<NetAddress>}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    dnses: Array<NetAddress>;

    /**
     * Routing information.
     * @type {Array<RouteInfo>}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    routes: Array<RouteInfo>;

    /**
     * Maximum transmission unit.
     * @type {number}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    mtu: number;
    /**
     * Whether the IPv4 address of the interface is valid.
     * @type { ?boolean }
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 24 dynamic&static
     */
    isIPv4LinkValid?: boolean;
    /**
     * Whether the IPv6 address of the interface is valid.
     * @type { ?boolean }
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 24 dynamic&static
     */
    isIPv6LinkValid?: boolean;
  }

  /**
   * Defines network route information.
   * @interface RouteInfo
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  export interface RouteInfo {
    /**
     * Network card name.
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    interface: string;

    /**
     * Destination Address
     * @type {LinkAddress}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    destination: LinkAddress;

    /**
     * Gateway address.
     * @type {NetAddress}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    gateway: NetAddress;

    /**
     * Whether a gateway is present.
     * @type {boolean}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    hasGateway: boolean;

    /**
     * Whether the route is the default route.
     * @type {boolean}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    isDefaultRoute: boolean;
  }

  /**
   * Defines network link information.
   * @interface LinkAddress
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  export interface LinkAddress {
    /**
     * Link address.
     * @type {NetAddress}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    address: NetAddress;
    /**
     * The length of the link address prefix.
     * @type {number}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    prefixLength: number;
  }

  /**
   * Defines a network address.
   * @interface NetAddress
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 8
   */
  export interface NetAddress {
    /**
     * Network address.
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    address: string;

    /**
     * Address family identifier. The value is 1 for IPv4 and 2 for IPv6. The default value is 1.
     * @type {?number}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    family?: number;

    /**
     * Port number. The value ranges from 0 to 65535.
     * @type {?number}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 8
     */
    port?: number;
  }

  /**
   * Network Global Proxy Configuration Information.
   * @interface HttpProxy
   * @syscap SystemCapability.Communication.NetManager.Core
   * @since 10
   */
  export interface HttpProxy {
    /**
     * Proxy server host name.
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 10
     */
    host: string;

    /**
     * Host port.
     * @type {number}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 10
     */
    port: number;

    /**
     * Do not use a blocking list for proxy servers.
     * @type {Array<string>}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @since 10
     */
    exclusionList: Array<string>;
  }

  /**
   * Network Interface Information.
   * @interface InterfaceInfo
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use.
   * @since 16
   */
  export interface InterfaceInfo {
    /**
     * Interface name.
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    interfaceName: string;

    /**
     * address of the interface.
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    address?: string;

    /**
     * Interface flags.
     * @type {number}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    flags?: number;

    /**
     * Interface scope.
     * @type {number}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    scope?: number;

    /**
     * if interface is up.
     * @type {boolean}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    up?: boolean;
  }

  /**
   * Network Route Change Information.
   * @interface RouteChangeInfo
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use.
   * @since 16
   */
  export interface RouteChangeInfo {
    /**
     * Interface name.
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    interfaceName: string;

    /**
     * route of the interface.
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    route: string;

    /**
     * gateway of the interface.
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    gateway: string;

    /**
     * if the route info is updated.
     * @type {boolean}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    updated: boolean;
  }

  /**
   * Network Interface Configuration Information.
   * @interface NetInterfaceConfiguration
   * @syscap SystemCapability.Communication.NetManager.Core
   * @systemapi Hide this for inner system use.
   * @since 16
   */
  export interface NetInterfaceConfiguration {
    /**
     * Interface name.
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    interfaceName: string;

    /**
     * Interface hardware address.
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    hwAddress: string;

    /**
     * Interface ipv4 address.
     * @type {string}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    ipv4Address: string;

    /**
     * Interface address prefixLength.
     * @type {number}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    prefixLength: number;

    /**
     * Interface flags.
     * @type {Array<string>}
     * @syscap SystemCapability.Communication.NetManager.Core
     * @systemapi Hide this for inner system use.
     * @since 16
     */
    flags: Array<string>;
  }
}

export default connection;
