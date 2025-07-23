/* SamsMapEditor.js
 * Sam-Sam uses "map" resources compatible with Egg's, but it does not use the grid data.
 * Instead, the geometry is entirely defined thru commands.
 * I need a visualization of this. Having done some initial layout as text, I can't see doing the whole scene that way.
 * All of our maps have a fixed height of 252 pixels. Should fit nicely if we scale up 2x.
 */
 
import { Dom } from "../js/Dom.js";
import { Data } from "../js/Data.js";
import { MapRes, MapCommand } from "../js/map/MapRes.js";
import { Namespaces } from "../js/Namespaces.js";
 
export class SamsMapEditor {
  static getDependencies() {
    return [HTMLElement, Dom, Data, Window, Namespaces];
  }
  constructor(element, dom, data, window, namespaces) {
    this.element = element;
    this.dom = dom;
    this.data = data;
    this.window = window;
    this.namespaces = namespaces;
    
    this.res = null;
    this.map = null;
    this.renderTimeout = null;
    this.graphics = null;
    this.selectedMapCommandId = 0;
    this.restoreScroll = 0;
    
    this.data.getImageAsync("graphics").then(img => this.graphics = img);
  }
  
  onRemoveFromDom() {
    if (this.renderTimeout) {
      this.window.clearTimeout(this.renderTimeout);
      this.renderTimeout = null;
    }
  }
  
  static checkResource(res) {
    if (res.type === "map") return 2;
    return 0;
  }
  
  setup(res) {
    this.res = res;
    this.map = new MapRes(res.serial);
    this.buildUi();
  }
  
  /* UI
   **********************************************************************************/
  
  buildUi() {
    this.element.innerHTML = "";
    
    const scroller = this.dom.spawn(this.element, "DIV", ["scroller"]);
    const visual = this.dom.spawn(scroller, "CANVAS", ["visual"], { "on-click": e => this.onClickVisual(e) });
    
    const bottom = this.dom.spawn(this.element, "DIV", ["bottom"]);
    const commandList = this.dom.spawn(bottom, "DIV", ["commandList"]);
    for (const command of this.map.commands) {
      const button = this.dom.spawn(commandList, "DIV", ["commandButton"], {
        "on-click": () => this.onChooseCommand(command.mapCommandId),
        "data-mapCommandId": command.mapCommandId,
      }, this.commandLabel(command));
      if (command.mapCommandId === this.selectedMapCommandId) button.classList.add("highlight");
    }
    const details = this.dom.spawn(bottom, "DIV", ["details"]);
    if (this.selectedMapCommandId) this.populateDetails();
    
    this.renderSoon();
  }
  
  commandLabel(command) {
    switch (command.tokens[0]) {
      case "width": return `width ${command.tokens[1].replace(/\(.*\)/g, "")}`;
      case "sprite": return `sprite ${command.tokens[3].replace(/\(.*\)/g, "")}`;
      case "platform": return `platform ${command.tokens[4].replace(/\(.*\)/g, "")}`;
    }
    return command.tokens[0];
  }
  
  populateDetails() {
    const parent = this.element.querySelector(".details");
    parent.innerHTML = "";
    const command = this.map.commands.find(c => c.mapCommandId === this.selectedMapCommandId);
    if (!command) return;
    const table = this.dom.spawn(parent, "TABLE");
    this.dom.spawn(table, "TR",
      this.dom.spawn(null, "TD", ["key"], "Command"),
      this.dom.spawn(null, "TD",
        this.dom.spawn(null, "INPUT", {
          type: "text",
          name: "command",
          value: command.encode(),
          "on-change": () => this.onCommandChange(),
        })
      )
    );
    this.dom.spawn(table, "TR",
      this.dom.spawn(null, "TD", ["key"], "Order"),
      this.dom.spawn(null, "TD",
        this.dom.spawn(null, "INPUT", { type: "button", value: "^", "on-click": () => this.moveCommand(command.mapCommandId, -1) }),
        this.dom.spawn(null, "INPUT", { type: "button", value: "v", "on-click": () => this.moveCommand(command.mapCommandId, 1) })
      )
    );
    // To edit, you have to touch the verbatim command and then blur it to commit.
    // It's not nearly as user-friendly as it could be, but hey this is quick n dirty for a game jam.
    // On second thought: Do let's allow numeric X and Y inputs when warranted. Those are tweaky and painful with just the command.
    // The three relevant commands all put X and Y in tokens 1 and 2 respectively.
    switch (command.tokens[0]) {
      case "hero":
      case "platform":
      case "sprite": {
          const tr = this.dom.spawn(table, "TR");
          this.dom.spawn(tr, "TD", ["key"], "Position");
          const tdv = this.dom.spawn(tr, "TD");
          this.dom.spawn(tdv, "INPUT", { type: "number", value: command.getInt(1, this.namespaces), name: "x", "on-input": () => this.onPositionChange() });
          this.dom.spawn(tdv, "INPUT", { type: "number", value: command.getInt(2, this.namespaces), name: "y", "on-input": () => this.onPositionChange() });
        } break;
    }
  }
  
  /* Events.
   **************************************************************************************/
   
  onChooseCommand(id) {
    for (const element of this.element.querySelectorAll(".commandButton.highlight")) {
      element.classList.remove("highlight");
    }
    const element = this.element.querySelector(`.commandButton[data-mapCommandId='${id}']`);
    if (element) {
      element.classList.add("highlight");
      this.selectedMapCommandId = id;
    } else {
      this.selectedMapCommandId = 0;
    }
    this.renderSoon();
    this.populateDetails();
  }
  
  onCommandChange() {
    const text = (this.element.querySelector("input[name='command']")?.value || "").trim();
    if (!text) { // Delete command.
      const cmdp = this.map.commands.findIndex(c => c.mapCommandId === this.selectedMapCommandId);
      if (cmdp < 0) return;
      this.map.commands.splice(cmdp, 1);
      const element = this.element.querySelector(`.commandButton[data-mapCommandId='${this.selectedMapCommandid}'`);
      if (element) element.remove();
      this.selectedMapCommandId = 0;
      this.populateDetails();
      this.renderSoon();
      this.data.dirty(this.res.path, () => this.map.encode());
    } else { // Replace command.
      const command = this.map.commands.find(c => c.mapCommandId === this.selectedMapCommandId);
      if (!command) return;
      command.decode(text);
      this.renderSoon();
      this.data.dirty(this.res.path, () => this.map.encode());
    }
  }
  
  onPositionChange() {
    const command = this.map.commands.find(c => c.mapCommandId === this.selectedMapCommandId);
    if (!command) return;
    const x = +this.element.querySelector("input[name='x']").value;
    const y = +this.element.querySelector("input[name='y']").value;
    command.tokens[1] = "(u16)" + x; // A more generic editor would preserve the prefix from existing token.
    command.tokens[2] = "(u16)" + y; // But we know it should always be "(u16)".
    this.element.querySelector("input[name='command']").value = command.encode();
    this.renderSoon();
    this.data.dirty(this.res.path, () => this.map.encode());
  }
  
  moveCommand(mapCommandId, d) {
    const p = this.map.commands.findIndex(c => c.mapCommandId === mapCommandId);
    if (p < 0) return;
    const command = this.map.commands[p];
    if (d < 0) {
      if (p <= 0) return;
      this.map.commands.splice(p, 1);
      this.map.commands.splice(p - 1, 0, command);
    } else {
      if (p >= this.map.commands.length - 1) return;
      this.map.commands.splice(p, 1);
      this.map.commands.splice(p + 1, 0, command);
    }
    this.buildUi(); // Heavy-handed, but whatever.
    this.data.dirty(this.res.path, () => this.map.encode());
  }
  
  onClickVisual(event) {
    // Of the dozen fucking different versions of (x,y) in (event), none of them accounts for scroll. "Position in element", why isn't that a thing???
    const scroller = this.element.querySelector(".scroller");
    const bounds = scroller.getBoundingClientRect();
    const scroll = scroller.scrollLeft;
    const scale = 2;
    const x = (event.x - bounds.x + scroll) / scale;
    const y = (event.y - bounds.y) / scale;
    
    // With Control, add a command. Use cent because it has to be something, and I expect to add lots of these.
    if (event.ctrlKey) {
      const cmd = new MapCommand(`sprite (u16)${~~x} (u16)${~~y} (u8:DECAL)cent (u8:flag)coin000 0x0000`);
      this.map.commands.push(cmd);
      this.selectedMapCommandId = cmd.mapCommandId;
      this.restoreScroll = this.element.querySelector(".scroller").scrollLeft;
      this.buildUi();
      this.data.dirty(this.res.path, () => this.map.encode());
      return;
    }
    
    // Iterate commands backward to the first match -- ones overlaying others visually take precedence.
    let mapCommandId = 0;
    for (let i=this.map.commands.length; i-->0; ) {
      const command = this.map.commands[i];
      if (this.pointInCommand(x, y, command)) {
        mapCommandId = command.mapCommandId;
        break;
      }
    }
    this.onChooseCommand(mapCommandId);
  }
  
  pointInCommand(x, y, command) {
    switch (command.tokens[0]) {
      case "hero": {
          const decal = DECALS.man;
          let dx = x - command.getInt(1, this.namespaces);
          dx += decal[3] >> 1;
          if ((dx < 0) || (dx >= decal[3])) return false;
          const dy = command.getInt(2, this.namespaces) - y;
          if ((dy < 0) || (dy >= decal[4])) return false;
          return true;
        }
      case "platform": {
          // You have to click a platform between its physical top and a 9-pixel height, like the bar (even for table, chair, etc).
          const px = command.getInt(1, this.namespaces);
          if (x < px) return false;
          const py = command.getInt(2, this.namespaces);
          if ((y < py) || (y > py + 9)) return false;
          const pw = command.getInt(3, this.namespaces);
          if (x >= px + pw) return false;
          return true;
        }
      case "sprite": {
          let dsrc = command.tokens[3] || "";
          if (!dsrc.startsWith("(u8:DECAL)")) return false;
          dsrc = dsrc.substring(10);
          const decal = DECALS[dsrc];
          if (!decal) return false;
          const dstx = command.getInt(1, this.namespaces) - (decal[2] >> 1);
          const dsty = command.getInt(2, this.namespaces) - decal[3];
          if ((x < dstx) || (x >= dstx + decal[2])) return false;
          if ((y < dsty) || (y >= dsty + decal[3])) return false;
          return true;
        }
    }
    return false;
  }
  
  /* Render.
   ************************************************************************************/
  
  renderSoon(to) {
    if (this.renderTimeout) return;
    this.renderTimeout = this.window.setTimeout(() => {
      this.renderTimeout = null;
      this.renderNow();
    }, to || 50);
  }
  
  renderNow() {
    if (!this.graphics) { // Beware this will retry forever if the image failed to load.
      this.renderSoon(250);
      return;
    }
    const canvas = this.element.querySelector(".visual");
    canvas.width = +this.map.getFirstCommand("width").split(')')[1];
    canvas.height = 252;
    const ctx = canvas.getContext("2d");
    ctx.fillStyle = "#ace";
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    for (const command of this.map.commands) {
      switch (command.tokens[0]) {
        case "indoors": this.renderIndoorsBorder(canvas, ctx); break;
        case "hero": this.renderHero(canvas, ctx, command); break;
        case "platform": this.renderPlatform(canvas, ctx, command); break;
        case "sprite": this.renderSprite(canvas, ctx, command); break;
      }
    }
    if (this.restoreScroll) {
      this.window.setTimeout(() => {
        this.element.querySelector(".scroller").scrollTo(this.restoreScroll, 0);
        this.restoreScroll = 0;
      }, 0);
    }
  }
  
  renderIndoorsBorder(canvas, ctx) {
    const [srcx, srcy, w, h] = DECALS.dollar;
    for (let x=0; x<canvas.width; x+=w) {
      ctx.drawImage(this.graphics, srcx, srcy, w, h, x, 0, w, h);
      ctx.drawImage(this.graphics, srcx, srcy, w, h, x, canvas.height - h, w, h);
    }
    for (let y=h; y<canvas.height; y+=h) {
      ctx.drawImage(this.graphics, srcx, srcy, w, h, 0, y, w, h);
      ctx.drawImage(this.graphics, srcx, srcy, w, h, canvas.width - w, y, w, h);
    }
  }
  
  highlight(ctx, x, y, w, h) {
    ctx.fillStyle = "#00f";
    ctx.globalAlpha = 0.75;
    ctx.fillRect(x - 3, y - 3, w + 6, h + 6);
    ctx.globalAlpha = 1;
  }
  
  renderHero(canvas, ctx, command) {
    const [srcx, srcy, w, h] = DECALS.man;
    const dstx = command.getInt(1, this.namespaces) - (w >> 1);
    const dsty = command.getInt(2, this.namespaces) - h;
    if (command.mapCommandId === this.selectedMapCommandId) this.highlight(ctx, dstx, dsty, w, h);
    ctx.drawImage(this.graphics, srcx, srcy, w, h, dstx, dsty, w, h);
  }
  
  renderPlatform(canvas, ctx, command) {
    const dstx = command.getInt(1, this.namespaces);
    const dsty = command.getInt(2, this.namespaces);
    const w = command.getInt(3, this.namespaces);
    const mode = command.tokens[4].replace("(u8:platform)", "").replace("0x0000", "");
    switch (mode) {
      case "arrow": {
          const [srcx, srcy, w2, h] = DECALS.arrow;
          if (command.mapCommandId === this.selectedMapCommandId) this.highlight(ctx, dstx, dsty, w, h);
          ctx.drawImage(this.graphics, srcx, srcy, w, h, dstx, dsty, w, h);
        } break;
      case "bed": {
          const [srcx, srcy, w2, h] = DECALS.bed;
          if (command.mapCommandId === this.selectedMapCommandId) this.highlight(ctx, dstx, dsty - 4, w, h);
          ctx.drawImage(this.graphics, srcx, srcy, w, h, dstx, dsty - 4, w, h);
        } break;
      case "table": {
          const [srcx, srcy, w2, h] = DECALS.table;
          if (command.mapCommandId === this.selectedMapCommandId) this.highlight(ctx, dstx, dsty, w, h);
          ctx.drawImage(this.graphics, srcx, srcy, w, h, dstx, dsty, w, h);
        } break;
      case "chair": {
          const [srcx, srcy, w2, h] = DECALS.chair;
          if (command.mapCommandId === this.selectedMapCommandId) this.highlight(ctx, dstx, dsty - 10, w, h);
          ctx.drawImage(this.graphics, srcx, srcy, w, h, dstx, dsty - 10, w, h);
        } break;
      default: { // "bar" or empty, also any errors. Not using the proper decals, only the dimensions matter.
          if (command.mapCommandId === this.selectedMapCommandId) this.highlight(ctx, dstx, dsty, w, 9);
          ctx.fillStyle = "#1f5d2b";
          ctx.fillRect(dstx, dsty, w, 9);
        }
    }
  }
  
  renderSprite(canvas, ctx, command) {
    let dsrc = command.tokens[3] || "";
    if (!dsrc.startsWith("(u8:DECAL)")) {
      console.log(`Unexpected decal id: ${JSON.stringify(command.tokens)}`);
      return;
    }
    dsrc = dsrc.substring(10);
    const decal = DECALS[dsrc];
    if (!decal) {
      console.log(`Unknown decal ${JSON.stringify(dsrc)} in command: ${JSON.stringify(command.tokens)}`);
      return;
    }
    const dstx = command.getInt(1, this.namespaces) - (decal[2] >> 1);
    const dsty = command.getInt(2, this.namespaces) - decal[3];
    if (command.mapCommandId === this.selectedMapCommandId) this.highlight(ctx, dstx, dsty, decal[2], decal[3]);
    ctx.drawImage(this.graphics, decal[0], decal[1], decal[2], decal[3], dstx, dsty, decal[2], decal[3]);
  }
}

/* Decals, matching src/game/decals.c.
 * sed -En 's/^  \[NS_DECAL_([0-9a-zA-Z_]+)\s*\]=\{ *([0-9]+), *([0-9]+), *([0-9]+), *([0-9]+).*$/  \1: [\2,\3,\4,\5],/p' src/game/decals.c
 * These are [x,y,w,h] in image:graphics.
 */
const DECALS = {
  dummy: [0,0,0,0],
  mattock: [1,1,13,13],
  shield: [15,1,12,14],
  sword: [28,1,16,16],
  shotgun: [45,1,16,7],
  bomb: [62,1,13,17],
  bow: [76,1,8,15],
  arrow: [85,1,5,12],
  speck: [91,1,3,1],
  ax: [95,1,10,26],
  bed: [106,1,27,13],
  basketball: [134,1,18,18],
  umbrella: [153,1,46,47],
  man: [200,1,16,32],
  door: [217,1,16,34],
  flowerpot: [51,9,10,9],
  table: [106,15,22,15],
  foot: [129,15,4,4],
  chair: [81,17,11,21],
  backpack: [1,18,30,30],
  tower: [32,18,14,26],
  workstation: [47,19,33,30],
  balloon: [129,20,9,10],
  bottle: [145,20,7,15],
  cent: [93,28,8,8],
  skull: [102,31,12,12],
  heart: [115,31,9,8],
  page: [200,34,9,14],
  edge: [210,36,6,9],
  platform: [217,36,14,9],
  hourglass: [129,38,9,12],
  dollar: [81,39,19,11],
  magnifier: [115,40,10,10],
  crown: [139,42,13,8],
  scroll: [213,46,9,13],
  dog: [162,49,26,29],
  candy: [189,49,12,11],
  widemouth: [202,49,10,14],
  house: [1,51,160,176],
  milk: [189,61,8,13],
  burger: [198,64,16,15],
  ivy: [173,82,9,33],
  earcup: [183,83,13,17],
  pottedplant: [162,90,10,25],
  book: [196,101,14,14],
  paddle: [183,103,12,12],
  cookie: [162,116,68,62],
  ghost: [162,179,52,48],
  earth: [1,228,169,169],
  weed: [171,236,14,12],
  weed2: [186,238,13,10],
  log: [171,249,29,21],
  moon: [171,271,39,32],
  pine: [171,304,31,43],
  sun: [171,348,50,49],
};
