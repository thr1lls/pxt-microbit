#include "pxt.h"

#define LOOKUP_PIN(x) X_PIN_##x
#define X_PIN_BTNMX_LATCH (&uBit.io.P9)
#define X_PIN_BTNMX_CLOCK (&uBit.io.P20)
#define X_PIN_BTNMX_DATA (&uBit.io.P14)

namespace pxt {

#define IS_3_3_V() ((NRF_UICR->REGOUT0 & 7) == 5)

static void disableNFConPins() {
    // Ensure NFC pins are configured as GPIO. If not, update the non-volatile UICR.
    if (NRF_UICR->NFCPINS || !IS_3_3_V()) {
        DMESG("RESET UICR\n");
        // Enable Flash Writes
        NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
            ;

        // Configure PINS for GPIO use.
        if (NRF_UICR->NFCPINS)
            NRF_UICR->NFCPINS = 0;

        // Set VDD to 3.3V
        if ((NRF_UICR->REGOUT0 & 7) != 5)
            NRF_UICR->REGOUT0 = (NRF_UICR->REGOUT0 & ~7) | 5;

        // Disable Flash Writes
        NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
            ;

        // Reset, so the changes can take effect.
        NVIC_SystemReset();
    }
}

static void waitABit() {
    // for (int i = 0; i < 10; ++i)
    //    asm volatile("nop");
}

class ButtonMultiplexer : public CodalComponent {
  public:
    Pin &latch;
    Pin &clock;
    Pin &data;
    uint32_t state;
    uint32_t invMask;
    uint16_t buttonIdPerBit[8];
    bool enabled;

    ButtonMultiplexer(uint16_t id)
        : latch(*LOOKUP_PIN(BTNMX_LATCH)), clock(*LOOKUP_PIN(BTNMX_CLOCK)),
          data(*LOOKUP_PIN(BTNMX_DATA)) {
        this->id = id;
        this->status |= DEVICE_COMPONENT_STATUS_SYSTEM_TICK;

        state = 0;
        invMask = 0;

        memset(buttonIdPerBit, 0, sizeof(buttonIdPerBit));

        disableNFConPins();
        
        data.getDigitalValue(PullMode::Down);
        latch.setDigitalValue(1);
        clock.setDigitalValue(1);
        enabled = true;
    }

    void disable() {
        data.getDigitalValue(PullMode::None);
        latch.getDigitalValue(PullMode::None);
        clock.getDigitalValue(PullMode::None);
        enabled = false;
    }

    bool isButtonPressed(int id) {
        for (int i = 0; i < 8; ++i) {
            if (buttonIdPerBit[i] == id)
                return (state & (1 << i)) != 0;
        }
        return false;
    }

    uint32_t readBits(int bits) {
        latch.setDigitalValue(0);
        waitABit();
        latch.setDigitalValue(1);
        waitABit();

        uint32_t state = 0;
        for (int i = 0; i < bits; i++) {
            state <<= 1;
            if (data.getDigitalValue(PullMode::Down))
                state |= 1;

            clock.setDigitalValue(0);
            waitABit();
            clock.setDigitalValue(1);
            waitABit();
        }

        return state;
    }

    virtual void periodicCallback() override {
        if (!enabled)
            return;

        uint32_t newState = readBits(8);
        newState ^= invMask;
        if (newState == state)
            return;

        for (int i = 0; i < 8; ++i) {
            uint32_t mask = 1 << i;
            if (!buttonIdPerBit[i])
                continue;
            int ev = 0;
            if (!(state & mask) && (newState & mask))
                ev = PXT_INTERNAL_KEY_DOWN;
            else if ((state & mask) && !(newState & mask))
                ev = PXT_INTERNAL_KEY_UP;
            if (ev) {
                Event(ev, buttonIdPerBit[i]);
                Event(ev, 0); // any key
            }
        }

        state = newState;
    }
};

static ButtonMultiplexer *btnMultiplexer;
ButtonMultiplexer *getMultiplexer() {
    if (!btnMultiplexer)
        btnMultiplexer = new ButtonMultiplexer(DEVICE_ID_FIRST_BUTTON);
    return btnMultiplexer;
}

int registerMultiplexedButton(int pin, int buttonId) {
    if (1050 <= pin && pin < 1058) {
        pin -= 50;
        getMultiplexer()->invMask |= 1 << (pin - 1000);
    }
    if (1000 <= pin && pin < 1008) {
        getMultiplexer()->buttonIdPerBit[pin - 1000] = buttonId;
        return 1;
    }
    return 0;
}

int multiplexedButtonIsPressed(int btnId) {
    if (btnMultiplexer)
        return btnMultiplexer->isButtonPressed(btnId) ? 512 : 0;
    return 0;
}

//% expose
uint32_t readButtonMultiplexer(int bits) {
    if (!LOOKUP_PIN(BTNMX_CLOCK))
        return 0;
    return getMultiplexer()->readBits(bits);
}

void disableButtonMultiplexer() {
    if (LOOKUP_PIN(BTNMX_CLOCK)) {
        getMultiplexer()->disable();
    }
}

//% expose
int pressureLevelByButtonId(int btnId, int codalId) {
    return multiplexedButtonIsPressed(btnId) ? 512 : 0;
}

//% expose
void setupButton(int buttonId, int key) {
    int pin = getConfig(key);
    if (pin == -1)
        return;
    pin &= 0xffff;
    registerMultiplexedButton(pin, buttonId);
}

} // namespace pxt