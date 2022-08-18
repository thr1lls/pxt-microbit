// Auto-generated. Do not edit.


declare interface ImageG {
    /**
     * Get the width of the image
     */
    //% property shim=ImageGMethods::width
    width: int32;

    /**
     * Get the height of the image
     */
    //% property shim=ImageGMethods::height
    height: int32;

    /**
     * True if the image is monochromatic (black and white)
     */
    //% property shim=ImageGMethods::isMono
    isMono: boolean;

    /**
     * Sets all pixels in the current image from the other image, which has to be of the same size and
     * bpp.
     */
    //% shim=ImageGMethods::copyFrom
    copyFrom(from: ImageG): void;

    /**
     * Set pixel color
     */
    //% shim=ImageGMethods::setPixel
    setPixel(x: int32, y: int32, c: int32): void;

    /**
     * Get a pixel color
     */
    //% shim=ImageGMethods::getPixel
    getPixel(x: int32, y: int32): int32;

    /**
     * Fill entire image with a given color
     */
    //% shim=ImageGMethods::fill
    fill(c: int32): void;

    /**
     * Copy row(s) of pixel from image to buffer (8 bit per pixel).
     */
    //% shim=ImageGMethods::getRows
    getRows(x: int32, dst: Buffer): void;

    /**
     * Copy row(s) of pixel from buffer to image.
     */
    //% shim=ImageGMethods::setRows
    setRows(x: int32, src: Buffer): void;

    /**
     * Return a copy of the current image
     */
    //% shim=ImageGMethods::clone
    clone(): ImageG;

    /**
     * Flips (mirrors) pixels horizontally in the current image
     */
    //% shim=ImageGMethods::flipX
    flipX(): void;

    /**
     * Flips (mirrors) pixels vertically in the current image
     */
    //% shim=ImageGMethods::flipY
    flipY(): void;

    /**
     * Returns a transposed image (with X/Y swapped)
     */
    //% shim=ImageGMethods::transposed
    transposed(): ImageG;

    /**
     * Every pixel in image is moved by (dx,dy)
     */
    //% shim=ImageGMethods::scroll
    scroll(dx: int32, dy: int32): void;

    /**
     * Stretches the image horizontally by 100%
     */
    //% shim=ImageGMethods::doubledX
    doubledX(): ImageG;

    /**
     * Stretches the image vertically by 100%
     */
    //% shim=ImageGMethods::doubledY
    doubledY(): ImageG;

    /**
     * Replaces one color in an image with another
     */
    //% shim=ImageGMethods::replace
    replace(from: int32, to: int32): void;

    /**
     * Stretches the image in both directions by 100%
     */
    //% shim=ImageGMethods::doubled
    doubled(): ImageG;

    /**
     * Draw given image on the current image
     */
    //% shim=ImageGMethods::drawImage
    drawImage(from: ImageG, x: int32, y: int32): void;

    /**
     * Draw given image with transparent background on the current image
     */
    //% shim=ImageGMethods::drawTransparentImage
    drawTransparentImage(from: ImageG, x: int32, y: int32): void;

    /**
     * Check if the current image "collides" with another
     */
    //% shim=ImageGMethods::overlapsWith
    overlapsWith(other: ImageG, x: int32, y: int32): boolean;
}
declare namespace image {

    /**
     * Create new empty (transparent) image
     */
    //% shim=image::create
    function create(width: int32, height: int32): ImageG;

    /**
     * Create new image with given content
     */
    //% shim=image::ofBuffer
    function ofBuffer(buf: Buffer): ImageG;

    /**
     * Double the size of an icon
     */
    //% shim=image::doubledIcon
    function doubledIcon(icon: Buffer): Buffer;
}

// Auto-generated. Do not edit. Really.
