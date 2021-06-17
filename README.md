# 2D Videogame
This repository contains the development of a 2D video game using the OpenGL API.

## Members of this project
- Alejandro, Kenny
- Hossain, Tanvir

## Features implemented
- 3 levels, each with 3 screens, with gradual difficulty
- 3 types of bricks:
    - first type: there are several colors, if it is touched then it will be destroyed
    - second type: destroyed by double tap
    - third type: it is more resistant, when touched it becomes the second type
- Point mechanics:
    - brick: 10 points, dollar: 100 points, money: 200 points, grapes: 500 points, diamond: 1000 points
- Mechanics of the key that allows to go from one screen of one level to the next
- Basic structure of 4 screens: main menu, play, instructions, credits
- Alarm that causes the appearance of a guard who pursues the player
- Interactivity of all entities
    - different types of bonuses: gray, green, blue, pink, (functions of the yellow and red coins are not implemented)
    - different types of player paddles: single, double, magnet 
    - interactivity with a ball
    - police
- Scrolling mechanics
- Keyboard keys to skip content:
    - key +: go to the top screen of the current one
    - key -: go to the bottom screen of the current one
    - n key: go to the next level of the current one
    - b key: go to the lower level of the current one
    - d key: changes from single to double paddle, or from double to single
    - k key: if the ball touches the lower limit of the first screen then it does not subtract life from the player; the ball will follow its direction
    - l key: to delete lives
    - g key: go from play to gameover
- Punctuation bar
- Sound and music effects with the irrKlang.dll library that allows to play audio tracks
- In the additional part we had planned to implement some kind of boss but it has not been implemented due to lack of time. 

## Game instructions
At the beginning we have the main screen of the game that consists of a navigable menu by means of the keys "p" to play, "i" to observe the instructions of the game, "c" to see the credits, "ESC" to exit and "b" to return from the instructions and credits screens, which will take you to the main menu.

Once we are in the game (previously we had to select the "p" key), the first screen of the first level is shown in which the components are highlighted: blocks, ball, key, bonus and the player himself (paddel + eyes).

To start the game we must press the "space" key, which throws the ball to destroy the blocks and get the key to go to the upper screens. If we want to go to the upper screen without having the key, we can do it using the god mode with the "+" key or return to that screen with the "-" key. In the last screen the player has the possibility to steal the money and leaving the bank, which implies passing the next level. However, if the alarm is activated then a policeman will appear to chase him and take a life from him, which implies starting the game from the initial screen of that level.

In the game the player has 4 lives to overcome the phases. If we are in the initial screen and the ball touches on the lower surface of the border limits then one life will be subtracted. If you want to ignore this effect, you can do it with the "k" key.

[See the demo](demo.avi)