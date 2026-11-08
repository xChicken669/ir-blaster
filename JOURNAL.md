---
title: "IR blaster"
github: "https://github.com/xChicken669/ir-blaster"
description: "IR blaster that runs on an esp32 c3 supermini\nIt uses 4 push buttons, an IR led and an IR receiver\n_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-"
created_at: "2026-10-01"
total_time: "14h"
---

# October 10, 2026: made the schematic

![image](https://fabricate.hackclub-assets.com/97694fa391f92e0e2a13f7055631c04fb533343bb64e8d394c643fd74e848c8c/image-png/fb925ebeeaf956d6e70d13a1f2651108ddad820f3a829b98e9bd4cd7a5048012.png)
I made a schematic in kicad
it took a while and this was the process:

so i started with adding the brain of the project which is the esp32 supermini and then i added an led, and a ir receiver which took a while to get because i had to find the specific model schematic and footprint, then i added a transistor between the led and the esp32 so it could output more current resulting in more range hopefully also then i added 4 different buttons which will be used to control the device, then i added the ssd1306 oled 128x64 display which will display the settings and menu yeah and that is probably it, this took me like 5 hours because of finding different symbols and footprints for kicad

![image](https://fabricate.hackclub-assets.com/d08e3917c4c1ac9a73498dc92cd470c1895b1ca498df4e6fa29c86ff4031edf8/image-png/7dc5b01a56bc5f6f740897adafcb4a248dec0010eb4d8fbe3580186a7244e29d.png)

![image](https://fabricate.hackclub-assets.com/e784f21799938294119a01b47cb70f682882da690e1c3d26e4540aac936abbe9/image-png/1c1a14c306a47b740ca48ae6c8191295c16bc8920757640bcb1d8edca10f0378.png)

also i assigned some 3d models to the different parts

![image](https://fabricate.hackclub-assets.com/58dfa5826ad095f00588fadec3d9f49452f64abb2f0f09287f680a1a9575d5e6/image-png/a43b22918820f1f034297533af3d860097b4e416f80ff8ccfff69d831d6db10e.png)

![image](https://fabricate.hackclub-assets.com/ccbea7ec8a8c602a44fa1326c69f4db36871abed24af8562f056164f6a589e82/image-png/1d55d953c5e49417d8599a0315eeee6e994c60d7a45b566e0255fd9e82e81c0e.png)

- [ir_blaster.kicad_sch](https://fabricate.hackclub-assets.com/b9d148a088a2b2b180fce301db13fc3664b572d2d1a6252870203e949bfddd6a/application-octet-stream/ir_blaster.kicad_sch)

**Total time spent: 5h**

# October 18, 2026: PCB + case

So I worked on this project almost all this day(about 7 hours), this is how it went:
I finished making the pcb schematic which was just making it more neat and I figured out the power management which will work like this: the pcb that i will be getting shipped if it gets approved+tp4056 batery charging module+battery
![](https://fabricate.hackclub-assets.com/252446d911146fd9cca0894c1df556b8ede47fadc54a44f848ccd3266de06796/image-png/image.png)

then the biggest part was probably making the pcb itself, the hardest part of that was aligning and positioning the components then there was routing which was pretty chill but sometimes it would overlap and i had to start again also i had to reasign some footprints and assign 3d models to the footprints which was just searching the internet for 3d models of the components
![](https://fabricate.hackclub-assets.com/1b8b1c8960db17186c6dc6e44fef9d320f70fc65ffdc4d5589f2cf8f94a96c94/image-png/image.png)

then i 3d modeled the case for it which was also pretty frustrating sometimes, at first i didnt know how to make it hold the pcb but i figured it out the case is split into 2 pieces, the top and the bottom
![](https://fabricate.hackclub-assets.com/5e101d4f276a7d5f9f9ff607e92a05049c3ae0efced329f039baee35f9dd21dc/image-png/image.png)

that is it, I will do the firmware next ;)

**Total time spent: 7h**

# November 7, 2026: FIRMWARE

I made the software using Arduino IDE yeah and i dont now what to really talk about here. I just wrote the code and i used help from chatgpt because i didnt know how to work with the codes i got for the different tv brands.Not that much tho
![](https://fabricate.hackclub-assets.com/d39c13bfe4611a7df5b78f1c90f34fcab9d9568491c3371db5668e0d096c3daf/image-png/image.png)
that is it

**Total time spent: 2h**
