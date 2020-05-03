#include "L0_Platform/startup.hpp"
#include "L1_Peripheral/stm32f10x/gpio.hpp"
#include "L2_HAL/memory/sd_experimental.hpp"
#include "utility/debug.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

const uint8_t kLongText[512] = R"(
Do you want to build a snowman?
Come on, let's go and play!
I never see you anymore
Come out the door
It's like you've gone away!
)";

const uint8_t kHelloWorld[] = "Hello World!";

class BitBangSpi : public sjsu::Spi
{
 public:
  BitBangSpi(sjsu::Gpio & sck, sjsu::Gpio & mosi, sjsu::Gpio & miso)
      : sck_(sck), mosi_(mosi), miso_(miso)
  {
  }

  sjsu::Status Initialize() const override
  {
    mosi_.SetAsOutput();
    miso_.SetAsInput();
    sck_.SetAsOutput();
    sck_.SetLow();
    mosi_.SetLow();
    miso_.GetPin().PullUp();
    return sjsu::Status::kSuccess;
  }

  uint16_t Transfer(uint16_t data) const override
  {
    uint16_t result = 0;
    for (int i = 0; i < 8; i++)
    {
      // Set MOSI pin to the correct bit
      mosi_.Set(sjsu::Gpio::State{ sjsu::bit::Read(data, 7 - i) });

      // Clock the SCK pin
      sck_.SetHigh();

      // Sample MISO pin
      result = static_cast<uint16_t>(result | miso_.Read() << (7 - i));

      // Pull clock pin low
      sck_.SetLow();
    }

    sck_.SetLow();
    return result;
  }

  void SetDataSize(DataSize size) const override
  {
    size_ = size;
  }

  void SetClock(units::frequency::hertz_t frequency,
                bool = false,
                bool = false) const override
  {
    if (frequency < 1_MHz)
    {
      delay_ = true;
    }
    else
    {
      delay_ = false;
    }
  }

 private:
  sjsu::Gpio & sck_;
  sjsu::Gpio & mosi_;
  sjsu::Gpio & miso_;
  mutable DataSize size_;
  mutable bool delay_ = false;
};

void SetMaximumClockSpeed()
{
  using sjsu::stm32f10x::SystemController;
  auto & config =
      sjsu::SystemController::GetPlatformController()
          .GetClockConfiguration<SystemController::ClockConfiguration>();

  // Set the speed of the high speed external oscillator.
  // NOTE: Change this if its different for your board.
  config.high_speed_external = 8_MHz;
  // Set the source of the PLL's oscillator to 4MHz.
  // See page 93 in RM0008 to see that the internal high speed oscillator,
  // which is 8MHz, is divided by 2 before being fed to the PLL to get 4MHz.
  config.pll.source = SystemController::PllSource::kHighSpeedExternal;
  // Enable PLL to increase the frequency of the system
  config.pll.enable = true;
  // Multiply the 8MHz * 9 => 72 MHz
  config.pll.multiply = SystemController::PllMultiply::kMultiplyBy9;
  // Set the system clock to the PLL
  config.system_clock = SystemController::SystemClockSelect::kPll;
  // APB1's maximum frequency is 36 MHz, so we divide 72 MHz / 2 => 36 MHz.
  config.ahb.apb1.divider = SystemController::APBDivider::kDivideBy2;
  // Keep APB1's clock undivided as it can handle up to 72 MHz.
  config.ahb.apb2.divider = SystemController::APBDivider::kDivideBy1;
  // Maximum frequency for ADC is 12 MHz, thus we divide 72 MHz / 6 to get
  // 12 MHz.
  config.ahb.apb2.adc.divider = SystemController::AdcDivider::kDivideBy6;

  // Initialize platform with new clock configuration settings.
  sjsu::InitializePlatform();
}

int main()
{
  sjsu::LogInfo("BEGIN SD Card Driver Example...");

  SetMaximumClockSpeed();

  sjsu::stm32f10x::Pin::ReleaseJTAGPins();

  sjsu::stm32f10x::Gpio miso('A', 6);
  sjsu::stm32f10x::Gpio mosi('A', 7);
  sjsu::stm32f10x::Gpio sck('A', 5);

  BitBangSpi spi(sck, mosi, miso);
  sjsu::stm32f10x::Gpio sd_chip_select('A', 4);

  sjsu::experimental::Sd card(spi, sd_chip_select,
                              sjsu::GetInactive<sjsu::Gpio>());

  // Must always initialize the hardware before usage.
  card.Initialize();

  if (card.IsMediaPresent())
  {
    sjsu::LogInfo("Card is" SJ2_HI_BOLD_GREEN " present!");
  }
  else
  {
    sjsu::LogInfo("Card is" SJ2_HI_BOLD_RED " NOT present!");
    sjsu::LogInfo("Please insert an SD card and restart the system.");
    return -1;
  }

  auto enable_status = card.Enable();
  if (!enable_status)
  {
    sjsu::LogError("Failed to mount SD Card!");
    return -2;
  }

  sjsu::LogInfo("Mounting of SD Card was" SJ2_HI_BOLD_GREEN " successful!");

  sjsu::LogInfo("Gather SD Card information...");
  units::data::gigabyte_t capacity = card.GetCapacity().value_or(0_B);
  units::data::byte_t block_size   = card.GetBlockSize().value_or(0_B);
  bool is_read_only                = card.IsReadOnly();

  sjsu::LogInfo("SD Card Capacity   = " SJ2_HI_BOLD_GREEN "%f GB",
                capacity.to<double>());
  sjsu::LogInfo("SD Card Block Size = " SJ2_HI_BOLD_GREEN "%d bytes",
                block_size.to<int>());
  sjsu::LogInfo("Is Read Only       = " SJ2_HI_BOLD_BLUE "%d", is_read_only);

  // Beyond this point, information on the SD card will be overwritten
  sjsu::LogInfo("Deleting blocks");
  SJ2_RETURN_VALUE_ON_ERROR(card.Erase(0, 1), -3);
  sjsu::Delay(1s);

  sjsu::LogInfo("Writing Hello World to block 0");
  SJ2_RETURN_VALUE_ON_ERROR(card.Write(0, kHelloWorld, sizeof(kHelloWorld)),
                            -4);
  sjsu::Delay(1s);

  sjsu::LogInfo("Reading block 0");
  uint8_t buffer[512];
  SJ2_RETURN_VALUE_ON_ERROR(card.Read(0, buffer, sizeof(buffer)), -5);
  sjsu::debug::Hexdump<8>(buffer, sizeof(buffer));
  sjsu::Delay(1s);

  sjsu::LogInfo("Deleting block 5");
  SJ2_RETURN_VALUE_ON_ERROR(card.Erase(5, 1), -6);

  sjsu::Delay(1s);

  sjsu::LogInfo("Reading block 5 after delete");
  SJ2_RETURN_VALUE_ON_ERROR(card.Read(5, buffer, sizeof(buffer)), -7);
  sjsu::debug::Hexdump<8>(buffer, sizeof(buffer));

  sjsu::LogInfo("Writing to block 5 after delete");
  SJ2_RETURN_VALUE_ON_ERROR(card.Write(5, kLongText, sizeof(kLongText)), -8);

  sjsu::Delay(1s);

  sjsu::LogInfo("Reading block 5 after write");
  SJ2_RETURN_VALUE_ON_ERROR(card.Read(5, buffer, sizeof(buffer)), -9);
  sjsu::debug::Hexdump<8>(buffer, sizeof(buffer));

  sjsu::LogInfo("End SD Card Driver Example...");
  return 0;
}