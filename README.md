# Sam-Sam

Requires [Egg](https://github.com/aksommerville/egg) to build.

Same Same Game Jam, 21 July 2025.
The idea is all the devs have to use the same set of provided graphics.

Modifications to the shared graphics:
 - Images are resliced and combined into one image. (the original had a few that were unreachable due to overlapping boxes).
 - The off-yellow pixels are all replaced with colors of my choosing.
 - All other (black and transparent) pixels are retained, except:
 - - Two pixels of the globe were transparent, seemingly by accident. I filled those in.
 - - The shotgun's outline was pure black. I made it #0e0e0e like the others.
 - Added a 3x5 G0 font in the Pacific Ocean, nominally obeying my rules, but definitely violating the spirit of them.
 - The intent of a few shapes was unclear so I took liberties and one can assume these were not the intended shapes:
 - - Sitting dog.
 - - Candy bar.
 - - Cheeseburger.
 - - Cheeseburger's wing.
 - - Cookie.
 - - Ghost wearing a backpack.
 
rishita wrote on Discord:
heres the deal: you must use only these provided visual assets. 
you have the freedom to modify them however you like, whether that means recoloring them, 
rearranging elements, or using them in new contexts. however, its important that your 
modifications dont drastically change the individual graphics to the point where 
they no longer resemble their original character or composition.

2025-07-22T07:05
A plan is taking shape. I've got the graphics colored, and they don't look so bad now.
Make a trivial platformer.
Pick up coins in the wild, trade them to the peddlar for things, and give those to the flying cheeseburger.
Once three things are given to the cheeseburger you win.
Um. It's not going to be all that engaging of a game.

## TODO

- [x] Gravity, jumping, and platform respect.
- [ ] Load scene from maps. Do use "map" resources, but disregard their grid data, use the command list only.
- [ ] Door.
- [ ] Coins to collect.
- [ ] Peddlar sprite.
- [ ] Peddlar modal.
- [ ] Cheeseburger.
- [ ] Setting and rising sun, moon, and cookie. Modify sky color on changes.
- [ ] Game over modal.
- [ ] Music.
- [ ] Sound effects.
