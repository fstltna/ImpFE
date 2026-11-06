Official Git Repository: https://github.com/fstltna/ImpFE

Official website: https://EmpireDirectory.net

ImpFE
========
ImpFE is a GUI client for the online multiplayer strategy game Imperium.  ImpFE uses GTK 3 for the GUI.

Building
--------

ImpFE needs a C compiler and the GTK 3 development files. On Debian/Ubuntu:

    sudo apt install build-essential pkg-config libgtk-3-dev
    make
    sudo make install

In Imperium players will be given a certain amount of money to pay for the expenses of operating a starship, and may try to increase their wealth by mining ore and gold, selling items to other players, etc.

Imperium can be played as a "team" game, with all players of a given race working togather, as a free-for-all, or any combination of the two.  Ships may be "programmed" to do simple actions repeatedly, and there are mining robots you can drop on an uninhabitable planet and pick up later.

Imperium is both "real time" and "turn" based. That is, if you happen to be playing at the same time as another player (or players), you will immediately be notified of any actions relating to you (and vice versa).  There is a built-in "chat" system & email system as well. To prevent people who have too much time on their hands from having too much of an advantage over those of us with real lives, there is also a "point" system for completing actions, which scales commands and limits what may be done in a given amount of "real time" before waiting for the points to build up again.

 ------------

ImpFE is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 1, or (at your option) any later version.

ImpFE currently has recently been tested on the following machines: Linux Ubuntu 12.x, Linux Ubuntu 13.x, Linux Ubuntu 14.x, Linux Ubuntu 16.04, Linux Ubuntu 20.04

And older versions: Linux 0.99pl10 (SLS), Linux 0.99pl12 (SLS), Linux 0.99pl12a (SLS), Linux 0.99pl13 (SlackWare), Linux 0.99pl14 (SlackWare), SCO Unix 3.2.2.0, SCO Unix 3.2.4.0, SCO Unix 3.2.4.1, SCO Unix 3.2.4.2, Solaris 2.5, Solaris 2.6, Solaris 2.7
