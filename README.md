# DayZ Server – Frostbyte (Sakhal)

Custom server configuration and scripting for a DayZ multiplayer environment focused on dynamic gameplay systems and player interaction.

## Overview

This project involves modifying and managing a DayZ server environment, including gameplay configuration, scripting, and system behavior. The goal was to create a more dynamic and reactive player experience through custom systems and event-driven behavior.

## Contribution

My primary contributions include:
- Developed custom spawn logic to generate randomized player loadouts, creating themed survivor roles
- Implemented a radio-based communication system to broadcast dynamic events such as weather alerts and player deaths
- Modified server configuration files to control environmental behavior, including weather, spawning, and gameplay systems
- Explored early-stage team systems and player communication mechanics
- Adjusted and tested real-time server behavior to influence player interaction and gameplay flow

## Concepts Explored

- Event-driven system behavior
- Real-time server configuration and interaction
- Player state and role assignment
- Conditional communication systems
- Environment (weather) and gameplay system tuning

## Project Structure

- scripts/ – Custom scripting and gameplay logic
- cfg* – Server configuration files for environment, spawning, and gameplay systems
- db/ – Server data and persistence
- tools/ – Utility scripts and helpers

## Development Status

Active configuration project with ongoing system experimentation and gameplay tuning. Partial work on team system and sprint game runs.

## Adjustments

### Ammo_Flare

```
<type name="Ammo_Flare">
<type name="Ammo_FlareBlue">
<type name="Ammo_FlareGreen">
<type name="Ammo_FlareRed">
```

Enabled flare ammo spawning in the Sakhal economy.

#### Changed
```
<nominal>0</nominal> -> <nominal>5</nominal>
<min>0</min>       -> <min>2</min>
```
#### Reason
Adds more atmospheric and survival-oriented lighting options for Sakhal without heavily impacting weapon balance.

#### File
```
db/types.xml
``` 

### Battery9V

```
<type name="Battery9V">
```
Increased 9V battery availability and average charge levels across Sakhal.

#### Changed

```
<nominal>30</nominal>  -> <nominal>60</nominal>
<min>25</min>          -> <min>30</min>
<quantmin>50</quantmin> -> <quantmin>85</quantmin>
```

Added:
```
<usage name="Town"/>
```

#### Effect

- Doubles the target amount of 9V batteries in the world
- Batteries now spawn with significantly higher average charge
- Batteries can now also spawn in town loot areas

Improves reliability of:
- Radios
- Flashlights
- Electronic equipment
- Night survival gameplay

#### Reason
Supports the server’s radio-focused atmosphere and improves electronic item usability in Sakhal’s harsher environment.

#### File
```
db/types.xml
```

### Flaregun
```
<type name="Flaregun">
```
Added increased flaregun spawn.

#### Changed

```
<nominal>0</nominal> -> <nominal>30</nominal>
<min>0</min> -> <min>25</min>
```

#### Reason
Flare gun need to be more prevalent in a survivor base zone with plenty of surrounding water.

#### File
```
db/types.xml
```

### GardenLime
```
<type name="GardenLime">
```

Disabled GardenLime spawning in the Sakhal economy.

#### Changed
```
<nominal>17</nominal> -> <nominal>0</nominal>
<min>12</min>         -> <min>0</min>
```

#### Effect

- Removes low-value farming clutter from loot spawns
- Reduces useless bulky items in farm loot areas
- Better fits Sakhal’s cold-weather survival focus

#### Reason
GardenLime has little practical use on this server because farming is not a major gameplay loop on Sakhal.

#### File
```
db/types.xml
```

### PersonalRadio
```
<type name="PersonalRadio">
```

Reduced loose PersonalRadio spawning while keeping radios important to gameplay.

#### Changed
```
<nominal>30</nominal> -> <nominal>20</nominal>
<min>25</min>         -> <min>10</min>
```

#### Effect

- Makes radios less common as random world loot
- Helps radios feel more valuable when found
- Supports server radio broadcasts without flooding the loot economy

#### Reason
Personal radios are part of the server’s gameplay identity, so they should matter without becoming disposable clutter.

#### File
```
db/types.xml
```

### Raincoat Control

Removing specific raincoat colors to control economy if teamplay is introduced.

Only Raincoats will spawn that are not part of the team color schemes.

#### Raincoats Removed
```
<type name="Raincoat_Blue">
<type name="Raincoat_Green">
<type name="Raincoat_Orange">
<type name="Raincoat_Red">
<type name="Raincoat_Yellow">
```

#### Increased Black and Pink coats
```
<type name="Raincoat_Black">
<type name="Raincoat_Pink">
```

#### Changes

```
<nominal>10</nominal> -> <nominal>0</nominal>
<min>3</min> -> <min>0</min>
```
Note: Green was the only value at 8.

```
<nominal>10</nominal> -> <nominal>25</nominal>
<min>3</min> -> <min>11</min>
```

#### Reason
Raincoat colors can conflict with team armband identity. Keeping only selected non-team colors preserves useful coastal rain gear without making team colors easy to swap in the wild.

#### File
```
db/types.xml
```

