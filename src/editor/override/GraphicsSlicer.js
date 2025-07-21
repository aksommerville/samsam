/* GraphicsSlicer.js
 * Performs a service we will need exactly once: Locate and enumerate the decals in our one image.
 * Each decal must be rectangles surrounded by a fence and all non-decal pixels must be the fence color.
 * This implies that the first pixel must be the fence color, and we'll depend on that.
 */
 
import { Data } from "../js/Data.js";
import { Dom } from "../js/Dom.js";
 
export class GraphicsSlicer {
  static getDependencies() {
    return [Data, Dom, Document];
  }
  constructor(data, dom, document) {
    this.data = data;
    this.dom = dom;
    this.document = document;
  }
  
  sliceGraphics() {
    const name = "graphics";
    this.data.getImageAsync(name).then(image => {
      const canvas = this.document.createElement("CANVAS");
      canvas.width = image.naturalWidth;
      canvas.height = image.naturalHeight;
      const ctx = canvas.getContext("2d");
      ctx.drawImage(image, 0, 0);
      const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
      this.sliceImageData(imageData, name);
    }).catch(e => this.dom.modalError(e));
  }
  
  sliceImageData(d, name) {
    if ((d.width < 1) || (d.height < 1)) throw new Error(`Unreasonable size for image ${JSON.stringify(name)}`);
    const d32 = new Uint32Array(d.data.buffer, d.data.byteOffset, d.width * d.height);
    const fenceColor = d32[0];
    let dst = "";
    /* To keep my head straight, I'm going to do this somewhat inefficiently:
     * Visit every pixel LRTB. When we find one that is not the fence color, measure the decal and record it.
     * Then fill that decal with the fence color.
     */
    for (let y=0, p=0; y<d.height; y++) {
      for (let x=0; x<d.width; x++, p++) {
        if (d32[p] === fenceColor) continue;
        const w = this.measureDecalRow(d32, p, d.width - x, fenceColor, x, y);
        const h = this.measureDecalHeight(d32, d.width, p, w, d.height - y, fenceColor, x, y);
        this.fill(d32, d.width, p, w, h, fenceColor);
        dst += `  {${x},${y},${w},${h}},\n`;
      }
    }
    console.log(dst);
  }
  
  measureDecalRow(d32, p, limit, fenceColor, x, y) {
    let w = 1;
    p++;
    for (;; w++, p++) {
      if (w >= limit) throw new Error(`${x},${y}: Missing right fence.`);
      if (d32[p] === fenceColor) return w;
    }
  }
  
  measureDecalHeight(d32, stride, p, w, limit, fenceColor, x, y) {
    let h = 1;
    p += stride;
    for (;; h++, p+=stride) {
      if (h >= limit) throw new Error(`${x},${y}: Missing bottom fence.`);
      if (d32[p] === fenceColor) return h;
      // We only check the leftmost column.
    }
  }
  
  fill(d32, stride, p, w, h, color) {
    for (; h-->0; p+=stride) {
      for (let xi=w, px=p; xi-->0; px++) {
        d32[px] = color;
      }
    }
  }
}

GraphicsSlicer.singleton = true;
