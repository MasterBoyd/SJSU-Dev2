// system clock class set clock outputs frequency and selects clock source
#pragma once

#include <cstdint>

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L1_Peripheral/system_controller.hpp"
#include "utility/build_info.hpp"
#include "utility/macros.hpp"
#include "utility/time.hpp"

namespace sjsu
{
namespace stm32f10x
{
class SystemController final : public sjsu::SystemController
{
 public:
  void SetSystemClockFrequency(
      units::frequency::megahertz_t frequency) const override
  {
  }

  void SetPeripheralClockDivider(const PeripheralID &,
                                 uint8_t peripheral_divider) const override
  {
  }

  uint32_t GetPeripheralClockDivider(const PeripheralID &) const override
  {
    return 1;
  }

  units::frequency::hertz_t GetSystemFrequency() const override
  {
    return 8_MHz;
  }

  /// Check if a peripheral is powered up by checking the power connection
  /// register. Should typically only be used for unit testing code and
  /// debugging.
  bool IsPeripheralPoweredUp(
      const PeripheralID & peripheral_select) const override
  {
    return true;
  }
  void PowerUpPeripheral(const PeripheralID & peripheral_select) const override
  {
  }
  void PowerDownPeripheral(
      const PeripheralID & peripheral_select) const override
  {
  }
};
}  // namespace stm32f10x
}  // namespace sjsu
