Isometriset graffat. Spriten leveys 64px korkeus 32px. Lattiaspritellä
lasketaan sijainti *yläreunasta* (jolloin lattian "paksuus" jää näkyviin) ja
hahmospriteillä ym. korkeilla spriteillä lasketaan sijainti *alareunasta* eli
kaikissa paitsi lattiaspriteissä on spriten alareuna samalla tasolla kuin
lattiaruudun alareuna. Tämä tarkoittaa että spriteä piirrettäessä voidaan
käyttää apuna valmista 64x32 ruutua.

Eli origo:
- Lattiaspriteissä 17. pikseli (0-indeksoituna pikseli 16) ylhäältä, vasen reuna
- Muissa spriteissä `korkeus - 15` pikseliä alhaalta

TODO:
 - pelaajasprite
 - vihu
