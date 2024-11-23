


### Particles 

Star particles are jusing instanced arrays to save on memory and the amount of draw calls to sent from the CPU to the GPU. 

Each star has the same position, which is then changed based on an offset that is randomly generated. These offsets are stored in an array, which is then sent to the vertex shader via vertex attributes. 


### Rain 


For the rain effect I will use the quad-billboarding technique, which is basically when you have a texture that is rotated to be perpendicular to the camera at all times. I want to try to implement this myself, in order test myself. 

For this I need a rain texture, and a quad where I can draw the texture. This will be needed for every drop. 

Later I might add a splash when it hits ground level, which will require some sort of sprite sheet. This could also be fun to implement. 


At first I did a billboarding technique which rotated around all axes, which is was not really what I wanted, at first at least. Heavy rain usually pours straight down. 

After I while I got it to work only rotating around the XZ axes, and is what I want for now, if wind is added, I might have to change this, so it can rotate around other axes as well. It was very difficult for me to figure out what vectors and directions to use, even though these are quite simple problems really. Got me feeling a bit sad. With ChatGPT it was solved without any issues however. I did take some time to try without, but I got very frustrated and just felt like I made no progress.

I will now try to get the rain moving, which can probably get interesting. 



### Backlog
- [x] Make Rain follow the player

- [ ] AABB
- [ ] Distance Fog
- [ ] Color gradients
- [ ] New BG
- [ ] Dirt ground
- [ ] Maybe some tree objects?


