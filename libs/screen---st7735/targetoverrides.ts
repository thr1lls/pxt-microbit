/**
 * Tagged image literal converter
 */
//% shim=@f4 helper=image::ofBuffer blockIdentity="images._spriteImage"
//% groups=["0.","1#","2T","3t","4N","5n","6G","7g","8","9","aAR","bBP","cCp","dDO","eEY","fFW"]
function img(lits: any, ...args: any[]): ImageG { return null }

// set palette before creating screen, so the JS version has the right BPP
//image.setPalette(hex`__palette`)
basic.showString("s")
image.setPalette(hex`000000ffffffff2121ff93c4ff8135fff609249ca378dc52003fad87f2ff8e2ec4a4839f5c406ce5cdc491463d000000`)
//% whenUsed
basic.showString("y")
const screen = _screen_internal.createScreen();
basic.showString("z")

namespace image {
    //% shim=pxt::setPalette
    export function setPalette(buf: Buffer) { }
}

namespace _screen_internal {
    //% shim=pxt::updateScreen
    function updateScreen(img: ImageG): void { }
    //% shim=pxt::updateStats
    function updateStats(msg: string): void { }

    //% shim=pxt::updateScreenStatusBar
    function updateScreenStatusBar(img: ImageG): void { return }
    //% shim=pxt::setupScreenStatusBar
    function setupScreenStatusBar(barHeight: number): void { return }

    export function createScreen() {
        const img = image.create(160, 120);
        setupScreenStatusBar(8);

        const status = image.create(160, 8)
        updateScreenStatusBar(status) // clear the status area

        control.__screen.setupUpdate(() => updateScreen(img))
        control.EventContext.onStats = function (msg: string) {
            status.fill(0)
            status.print(msg, 2, 2, 1, image.font5)
            updateScreenStatusBar(status)
            updateStats(msg);
        }

        return img as ScreenImage;
    }

}
