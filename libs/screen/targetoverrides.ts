const screen = _screen_internal.createScreen();

namespace image {
    //% shim=pxt::setPalette
    export function setPalette(buf: Buffer) { }
}

namespace _screen_internal {

    export function createScreen(): ImageG {
        return null
    }

}

