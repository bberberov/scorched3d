----------------------------------------------------
             Apocalypse v4.1 Information
----------------------------------------------------
Apocalypse is a modification for Scorched 3D which adds many new weapons, items, and landscapes
for use with this awesome game.  The mod is the product of work by several people (named in authors.txt).
All original work contained in the mod is copyright.  Legal information can be found in license.txt.


Official Apocalypse Site:  http://apochq.handwired.net
Official Scorched 3D Site:  http://scorched3d.co.uk

Changes Since 4.0
    New Accessories:
        - Mirror Shield - prevents damage from lasers but only provides 5 pts of protection from explosions
        
    Other Accessory Changes:
        - Laser Cannon
            - Has random spread from -0.5 to 0.5 degrees on 2nd and 3rd shot
            - Does 60 to 120 damage instead of 75 to 150 damage.
        - Tweaked the nuclear fallout sparkle effect a bit (spread out more)
        - Added the fallout sparkle effect to Tsar Bomba
        - Spaced out the bomblets in Funky Bomb over time slightly to prevent dudding on direct hit
        - Renamed 'Small Battery' drop to 'Batteries'
		- Lighting values adjusted on most models so it looks okay with the new lighting changes
        - Shield pricing tweaked slightly to reduce dominance of the force shields
        - Cargo drops now fall a bit slower
        - New position selection method applied to transporter
        
    Landscapes:
    	- Cities
            - Factories now release some additional acid bomblets when you destroy them
            - Hospital now drop 0-3 medkits instead of 1-3 but gives 25 health when you destroy it
            - Stadium can now drop up to $20,000 instead of $15,000
            - Most buildings are now targets (projectiles will collide)
        - Buildings and/or cargo drops added to a few more landscapes/texture layouts
        - Random flame spurts (geysers) that pop up out of the ground on the hell texture layout
        - Weather events can now damage tanks
        - Increased likelyhood of cargo drops on landscapes that have them
        - Igloos added to ice_bldg and ice texture layouts
            - Drops same items as stores at the moment
        - Fleshy Pods added to hell texture layout
            - Drops medkits/parachutes/batterys and occasionally cargo boxes
        - Acid Pods added to hell texture layout
            - Spreads small acid droplets when destroyed
            - Drops small weapons and occasionally cargo boxes
        - Soldiers standing around the Nuclear Plants
        - Converted Igloos to targets
        - Increased max altitude of boids on city maps slightly
        - Random satellite crashes on moon maps that drop cargo
        
    Other:
        - Various other small changes and fixes
        - Fixed single player game settings
        - Darkened map in the plan view a bit
        - GUI windows are now slightly transparent
        
Notes About Installation:
The Apocalypse Mod comes with Scorched 3D and, as a result, you should not need to install anything.  However, 
in the event that you are re-installing the mod because it has become corrupt, it should be installed into the
directory you installed Scorched3D to, under /data/globalmods/.  You should delete what is in the Apoc directory
in your globalmods folder and in your ~/.scorched3d/mods/apoc/ folder before re-installing for a completely clean
installation.  This will prvent any issues caused by having obselete or altered files in your Apoc install.

Updates for the mod can typically be obtained on the official Apocalypse servers at ApocHQ, or by going to 
http://apochq.handwired.net.  

        
----------------------------------------------------
                     Files
----------------------------------------------------
authors.txt         - Contains the names of all contributors to the Apocalypse Mod, and what they contributed
license.txt         - License for the mod and related materials
readme.txt          - This file
readme-nexus.txt    - Readme file that comes with Nexus6's maps

data/               - Directory containing all data files and other media used by the mod
accessories.xml     - File containing all the item/weapons for the game
ainames.txt         - Contains the custom names for the bots (all names of countries for Apocalypse)
landscapes.xml      - File containing basic landscape data
server.xml          - Example Server settings for running an Apocalypse Server
singletarget.xml    - Default target practice game settings
singleeasy.xml      - Default easy difficulty single player game settings
singlenormal.xml    - Default normal difficulty single player game settings
singlehard.xml      - Default hard difficulty single player game settings
singlegames.xml     - Defines menu icons/text for the mod
tankais.xml         - File containing parameters for the ai players
textureset.xml      - Definitions for the textures used in explosion animations

data/accessories/*  - Custom models and their textures used by the mod
data/landscapes/*   - All the custom heightmaps, pictures and xml files for Apocalypse landscapes
    ambientsound*.xml - Stores ambient sound groupings
    defn*.xml       - Info regarding the shape of a landscape
    place*.xml      - Info regarding placement of the landscape objects
    tex*.xml        - Defines all the texture layouts that are used by the landscapes
data/meshes/*   - Boid models used by the mod
data/textures/* - All custom weapon icons, explosion textures or landscape textures used by the mod
data/wav/*      - All custom sounds used by the mod
data/windows/*  - GUI windows

Note: Server settings are not required but are provided as an example

----------------------------------------------------
                  Apocalypse Weapons
----------------------------------------------------
Acid Blast      - Explodes releasing some small, earth eating bomblets
Acid Rain       - Non damaging, earth eating rain
Acid Splash     - Explodes on impact and splashes dirt eating bomblets all over the area
Apocalypse      - Warhead that splits a few times in the air realeasing many medium sized nuclear warheads
Baby Napalm     - Weak version of napalm
Blink Device    - Prototype, near-instant teleporter.  Portal created is unstable.
Bottomless Pit  - Creates a narrow but deep pit at the point of impact
BouncingBetty   - Bomb that bounces and releases multiple explosive warheads
Bunny Bomb      - MIRV with leaping warheads
Chain Lightning - Explodes and releases an aimed arc of electricity with courses through the ground aimed at nearby tanks
Dmsday Device   - Larger, more destructive version of Apocalypse
Earthquake      - Shakes the earth, opens up a fissure and does damage over a large area from a random location on the map.
EMP Grenade     - Grenade that does heavy damage to shields and light damage to tanks
EMP Missile     - Missile with a larger EMP effect than the grenade
Fatman          - In between a nuke and a MOAB
Fire Ants       - MIRV that releases multiple rolling, explosive napalm warheads
Fire Worms      - Creates a series of slow-moving, explosive napalm diggers
Flame Thrower   - Shoots a stream of napalm at the target
Fork Lightning  - Larger version of chain lightning which splits up into several arcs as it progresses
Gas Grenade     - Lands, bounces and releases a cloud of harmful gas.  Custom model by ShoCkwaVe
Haymaker        - MIRV with Bouncing Betty warheads
Hellstorm       - Firey/explosive rain
Hornet's Nest   - Creates an angry cloud of hornets which fly out and attack nearby tanks
Hornet Swarm    - Releases multiple Hornet's Nests
Laser Cannon    - Fires 3 regular laser shots at slightly different angles
Lightning Bolt  - Fires from a random location creating a small cloud and lightning bolt
Lightning Storm - Fires from a random location creating several lightning bolts over a few seconds
M2 Machinegun   - Shoots five 50 caliber machinegun bullets in rapid succession.
Mega EMP        - Larger, more powerful version of the EMP grenade
Mega Meteor     - Larger, more powerful version of meteor
Mega MIRV       - Larger, more powerful version of the standard MIRV
Mega S-MIRV     - Larger, more powerful version of the spread MIRV
Meteor          - Meteor appears at the peak of flight and lands scattering flaming earth
Meteor Shower   - Fires from a random location and creates many meteors that rain down upon everyone
MOAB            - Large conventional explosive bomb
Napalm Blast    - Explodes on impact and releases many small napalm bomblets
Napalm MIRV     - MIRV with napalm warheads
Napalm S-MIRV   - Spread MIRV with napalm warheads
Quicksand       - Explodes on impact releasing a series of slow-moving, poorly aimed diggers
Riot Drill      - 3 heavy riot bombs are shot in rapid succession at the same point
Sat. Strike     - Shoots a projectile which paints the target area.  Space based lasers then assault the target.
Shotgun         - Shoots 6 pellets in a random spread with limited range/line of sight
Shockwave       - Creates a massive energy wave which ripples outwards without destroying the landscape
Strangelove     - The end game weapon, scaled up dmsday device
Suicide Bomb    - Detonates yourself causing a huge, powerful explosion
The Travolta    - Super-sized Funky Bomb
Transporter     - Transports the user to the location of a marker projectile
Tsar Bomba      - Largest single nuclear explosive bomb ever made
Volcano         - Creates a volcano that erupts upon impact
Wall of Dirt    - Raises a small wall of dirt a short distance from your tank in the direction you aim
X Tracer        - Shoots a spread of tracers at 2.5 degree increments

----------------------------------------------------
                     New Items
----------------------------------------------------
Hv Frc Shield   - Larger, more powerful version of the force shield
Ultra Shield    - Most powerful standard defensive shield
Heavy Mag       - Larger version of the mag deflect
Light Armor     - Provides an extra 25 points of protection
Heavy Armor     - Provides an extra 50 points of protection

----------------------------------------------------
                  Apocalypse Maps
----------------------------------------------------
Maps By Bobirov:
    BigHill     - Random:  Generally a single very large hill
    Mountains   - Random:  Very mountainous map
    Ring        - Random:  Small valley or pond always in the middle
    Apoc City   - Static:  Island map that comes complete with a sprawling city to destroy
    World Map   - Static:  Map of the entire planet
    USA         - Static:  Map of the continental U.S.
    Bullseye    - Static:  Round medium-height map with a few elevation levels, city-enabled
    Mesa City   - Static:  Square map with a high, flat surface, city-enabled
    Pillars     - Static:  Series of steep outcroppings of various heights
    Colloseum   - Static:  Round "arena-like" map
    Ruffles     - Static:  Map that looks pretty much like a ruffles potato chip
    Maze        - Static:  A nice maze with players spawning only inside on the ground level
    Volcano     - Static:  An volcanic tropical island.

Maps By Nexus6:
    Castle      - Static:  Sand castle looking map
    Moon        - Static:  Nice hilly, moon like map
    Pyramids    - Static:  Moderate height map with some nice pyramids to blow up
    Weathered   - Static:  Another nice hilly map
    Hex World   - Static:  Cool map made up of hexagonal shapes of varying heights
    Two Towers  - Static:  Nice map with two large round towers
    Craters     - Static:  Craterous moon map with a nice view of the earth
    Pyramids 2  - Static:  Hand remake of the original Pyramids map
    Desert Mesa - Static:  Southwestern US style map
    Terraces    - Static:  Landscape made up of various height levels
    Steppes     - Static:  Similar to terraces but with a different shape
    Canyon      - Static:  Deep canyon to annihilate eachother in
    
Maps By ShoCkwaVe:
    ArizonaIslands  - Static:  Series of islands that resemble rock formations in the western U.S.

----------------------------------------------------
                     Legal Stuff
----------------------------------------------------
Here is the human readable version of the license. If you are a lawyer or
just like reading legal documents, take a look at the contents of the
LICENSE file.

Creative Commons Attribution-ShareAlike 3.0

You are free:

    * to Share — to copy, distribute and transmit the work
    * to Remix — to adapt the work

Under the following conditions:

    * Attribution. You must attribute the work in the manner specified by the author or licensor
      (but not in any way that suggests that they endorse you or your use of the work).
    * Share Alike. If you alter, transform, or build upon this work, you may distribute the resulting
      work only under the same, similar or a compatible license.

    * For any reuse or distribution, you must make clear to others the license terms of this work. 
      The best way to do this is with a link to this web page.
    * Any of the above conditions can be waived if you get permission from the copyright holder.
    * Nothing in this license impairs or restricts the author's moral rights.

