# BulletLeague3D
BulletLeague3D is car physics based game that takes inspiration from Rocket League.
This is an student project made by sophomore students from CITM.

## Objectives
The objective of BulletLeague3D is to be a tribute to the Rocket League style of gameplay (fiest introduced to the market by SSper Snic Acrobatic Battle Powered Cars). The main goal of our project is to have two cars (and thus, multiple cameras) that beheave like the ones of Rocket League (jump, boost, flips, flying,...), a field of "Rocke League-like football" & a game loop according that represents it's gameplay.

## Guide to Finish the Game
Every match lasts 5min; whoever scores more wins the match. After a match is over, the score of each player is reset & a new match starts. So, to finish the game you have to beat your oponent!
To score a goal, you must push the ball fully into your oponen's goal; when a goal is performed, a new round starts. This is our main game loop & it repeats itself until the match is finished & a new one starts.

In case of a draw, everybody is a winner:)

## Link to our Github Project:
https://github.com/AaronGCProg/BulletLeague3D


## Game Keys:

### Player 1 Keyboard control:

- W: Move foward
- A: Turn left
- D: Turn right
- S: Move backwards
- Space: Jump (When jumping, you can perform an additional jump that can become a directional flip toward a direction!)
- B: Break
- Left-Shift: Turbo
- R: Change Camera Mode (Player focused - Ball Focused)

### Player 2 Keyboard control:

- Arrow UP: Move foward
- Arrow LEFT: Turn left
- Arror RIGHT: Turn right
- Arrow DOWN: Move backwards
- NUMPAD 0: Jump (When jumping, you can perform an additional jump that can become a directional flip toward a direction!)
- NUMPAD 1: Break
- Right-Shift: Turbo
- NUMPAD 5: Change Camera Mode (Player focused - Ball Focused)

> WARNING: If you found yourself flipped down, press the foward key + any turn direction to return to the regular position.
> If this doesn't work (99.9% does work thought) please press the asignated Debug Key to Reset the car.

- Esc: Exit game

### Debug Options:

- F1:  Debug Mode
- F2:  Reset Blue Car 
- F3:  Reset Orange Car 
- F4:  Reset Ball
- F5:  Reset Goal 
- F6:  Reset Match
- F9: Debug Camera Mode

## Credits
- Aarón Guerrero Cruz
> Github profile: https://github.com/AaronGCProg

- Àlex Melenchón Maza
> Github profile: https://github.com/AlexMelenchon


## License:
MIT License

Copyright (c) [2019] [Aarón Guerrero Cruz / Àlex Melenchón Maza]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
