#include "SPIScreenIO.h"

#include "pxt.h"
#include "ST7735.h"
#include "ILI9341.h"

#define LOOKUP_PIN(x) X_PIN_##x
#define X_PIN_DISPLAY_MISO (&uBit.io.P14)
#define X_PIN_DISPLAY_MOSI (&uBit.io.P15)
#define X_PIN_DISPLAY_SCK (&uBit.io.P13)
#define X_PIN_DISPLAY_BL (&uBit.io.P19)
#define X_PIN_DISPLAY_RST (&uBit.io.P16)
#define X_PIN_DISPLAY_DC (&uBit.io.P8)
#define X_PIN_DISPLAY_CS ((codal::Pin *)NULL)

#define CODAL_SPI NRF52SPI

extern uint32_t readButtonMultiplexer(int bits);

namespace pxt {

int waitForScreen() {
    uint32_t hc;

    // wait while nothing is connected
    for (;;) {
        auto rst = LOOKUP_PIN(DISPLAY_RST);
        if (rst) {
            rst->setDigitalValue(0);
            target_wait_us(10);
            rst->setDigitalValue(1);
            fiber_sleep(3); // in reality we need around 1.2ms
        }

        fiber_sleep(100);

        hc = readButtonMultiplexer(17);
        if (hc != 0)
            break;

        fiber_sleep(100);
    }

    return hc;
}


class WDisplay {
  public:
    ScreenIO *io;
    ST7735 *lcd;

    uint32_t currPalette[16];
    bool newPalette;
    bool inUpdate;

    uint8_t *screenBuf;
    ImageG_ lastStatus;

    uint16_t width, height;
    uint16_t displayHeight;
    uint8_t offX, offY;
    bool doubleSize;
    uint32_t palXOR;

    WDisplay() {
        uint32_t cfg0 = 0x00000040; // getConfig(CFG_DISPLAY_CFG0, 0x40);
        uint32_t frmctr1 = 0x00000603; // getConfig(CFG_DISPLAY_CFG1, 0x000603);
        int dispTp = DISPLAY_TYPE_ST7735; // DISPLAY_TYPE_SMART; // getConfig(CFG_DISPLAY_TYPE, DISPLAY_TYPE_SMART); 

        auto hc = waitForScreen();
        if (hc == 0x1FFFF) {
            target_panic(42);
        }

        doubleSize = false;
        auto miso = LOOKUP_PIN(DISPLAY_MISO);
        miso = NULL;
        SPI *spi = new CODAL_SPI(*LOOKUP_PIN(DISPLAY_MOSI), *miso, *LOOKUP_PIN(DISPLAY_SCK));
        io = new SPIScreenIO(*spi);

        lcd = new ST7735(*io, *LOOKUP_PIN(DISPLAY_CS), *LOOKUP_PIN(DISPLAY_DC));

        palXOR = (cfg0 & 0x1000000) ? 0xffffff : 0x000000;
        auto madctl = cfg0 & 0xff;
        offX = (cfg0 >> 8) & 0xff;
        offY = (cfg0 >> 16) & 0xff;

        DMESG("configure screen: FRMCTR1=%p MADCTL=%p type=%d", frmctr1, madctl, dispTp);

        auto freq = 32;
        spi->setFrequency(freq * 1000000);
        spi->setMode(0);

        // make sure the SPI peripheral is initialized before toggling reset
        spi->write(0);

        auto rst = LOOKUP_PIN(DISPLAY_RST);
        if (rst) {
            rst->setDigitalValue(0);
            fiber_sleep(20);
            rst->setDigitalValue(1);
            fiber_sleep(20);
        }

        auto bl = LOOKUP_PIN(DISPLAY_BL);
        if (bl) {
            bl->setDigitalValue(1);
        }
        lcd->init();
        lcd->configure(madctl, frmctr1);

        width = 160;  // getConfig(CFG_DISPLAY_WIDTH, 160);
        height = 128; // getConfig(CFG_DISPLAY_HEIGHT, 128);
        displayHeight = height;
        setAddrMain();
        DMESG("screen: %d x %d, off=%d,%d", width, height, offX, offY);
        int sz = doubleSize ? (width >> 1) * (height >> 1) : width * height;
        screenBuf = (uint8_t *)app_alloc(sz / 2 + 20);

        lastStatus = NULL;
        registerGC((TValue *)&lastStatus);
        inUpdate = false;
    }

    void setAddrStatus() {
        lcd->setAddrWindow(offX, offY + displayHeight, width, height - displayHeight);
    }
    void setAddrMain() {
        lcd->setAddrWindow(offX, offY, width, displayHeight);
    }
    void waitForSendDone() {
        lcd->waitForSendDone();
    }
    int sendIndexedImage(const uint8_t *src, unsigned width, unsigned height, uint32_t *palette) {
        return lcd->sendIndexedImage(src, width, height, palette);
    }
};

SINGLETON(WDisplay);

//%
int setScreenBrightnessSupported() {
    return 0;
}

//%
void setScreenBrightness(int level) {

}

//%
void setPalette(Buffer buf) {
    auto display = getWDisplay();
    if (!display)
        return;
    if (48 != buf->length)
        target_panic(PANIC_SCREEN_ERROR);
    for (int i = 0; i < 16; ++i) {
        display->currPalette[i] =
            (buf->data[i * 3] << 16) | (buf->data[i * 3 + 1] << 8) | (buf->data[i * 3 + 2] << 0);
        display->currPalette[i] ^= display->palXOR;
    }
    display->newPalette = true;
}

//%
void setupScreenStatusBar(int barHeight) {
    auto display = getWDisplay();
    if (!display)
        return;
    if (!display->doubleSize) {
        display->displayHeight = display->height - barHeight;
        display->setAddrMain();
    }
}

//%
void updateScreenStatusBar(ImageG_ img) {
    auto display = getWDisplay();
    if (!display)
        return;

    if (!img)
        return;
    display->lastStatus = img;
}

//%
void updateScreen(ImageG_ img) {
    auto display = getWDisplay();
    if (!display)
        return;

    if (display->inUpdate)
        return;

    display->inUpdate = true;

    auto mult = display->doubleSize ? 2 : 1;

    if (img) {
        if (img->bpp() != 4 || img->width() * mult != display->width ||
            img->height() * mult != display->displayHeight)
            target_panic(PANIC_SCREEN_ERROR);

        // DMESG("wait for done");
        display->waitForSendDone();

        auto palette = display->currPalette;

        if (display->newPalette) {
            display->newPalette = false;
        } else {
            palette = NULL;
        }

        memcpy(display->screenBuf, img->pix(), img->pixLength());

        // DMESG("send");
        display->sendIndexedImage(display->screenBuf, img->width(), img->height(), palette);
    }

    if (display->lastStatus && !display->doubleSize) {
        display->waitForSendDone();
        img = display->lastStatus;
        auto barHeight = display->height - display->displayHeight;
        if (img->bpp() != 4 || barHeight != img->height() || img->width() != display->width)
            target_panic(PANIC_SCREEN_ERROR);
        memcpy(display->screenBuf, img->pix(), img->pixLength());
        display->setAddrStatus();
        display->sendIndexedImage(display->screenBuf, img->width(), img->height(), NULL);
        display->waitForSendDone();
        display->setAddrMain();
        display->lastStatus = NULL;
    }

    display->inUpdate = false;
}

//%
void updateStats(String msg) {
    // ignore...
}

} // namespace pxt