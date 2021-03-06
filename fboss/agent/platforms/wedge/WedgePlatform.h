/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

#include "fboss/agent/hw/bcm/BcmPlatform.h"
#include "fboss/agent/types.h"
#include "fboss/agent/platforms/wedge/WedgeProductInfo.h"
#include "fboss/agent/platforms/wedge/WedgeI2CBusLock.h"

#include <folly/MacAddress.h>
#include <boost/container/flat_map.hpp>
#include <memory>
#include <unordered_map>

namespace facebook { namespace fboss {

class BcmSwitch;
class WedgePort;

class WedgePlatform : public BcmPlatform {
 public:
  explicit WedgePlatform(std::unique_ptr<WedgeProductInfo> productInfo);
  WedgePlatform(std::unique_ptr<WedgeProductInfo> productInfo,
                uint8_t numQsfps);
  ~WedgePlatform() override;

  void init();

  HwSwitch* getHwSwitch() const override;
  void onHwInitialized(SwSwitch* sw) override;
  std::unique_ptr<ThriftHandler> createHandler(SwSwitch* sw) override;

  folly::MacAddress getLocalMac() const override;
  std::string getVolatileStateDir() const override;
  std::string getPersistentStateDir() const override;

  void onUnitAttach() override;
  InitPortMap initPorts() override;
  void getProductInfo(ProductInfo& info) override;

  bool canUseHostTableForHostRoutes() const override {
    // Making this false till we can warm boot across this
    // change - t8067423
    return false;
  }

 protected:
  typedef boost::container::flat_map<PortID, std::unique_ptr<WedgePort>>
    WedgePortMap;

  WedgePortMap ports_;

 private:
  // Forbidden copy constructor and assignment operator
  WedgePlatform(WedgePlatform const &) = delete;
  WedgePlatform& operator=(WedgePlatform const &) = delete;

  void initLocalMac();
  WedgePlatformMode getMode();
  virtual std::map<std::string, std::string> loadConfig();
  virtual std::unique_ptr<BaseWedgeI2CBus> getI2CBus();
  void initTransceiverMap(SwSwitch* sw);

  folly::MacAddress localMac_;
  std::unique_ptr<BcmSwitch> hw_;

  const std::unique_ptr<WedgeProductInfo> productInfo_;
  uint8_t numQsfpModules_{0};
  std::unique_ptr<WedgeI2CBusLock> wedgeI2CBusLock_;
};

}} // namespace facebook::fboss
