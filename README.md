# HoverVideoPlayer
It's a basic video player that has a hover feature like youtube. When you take your cursor to slide bar, it shows de frame.
Do not forget, main purpose is hover feature. So, not even trying to add some render features like in VLC.

Will be edited:
  Design:
    Especially in fullscreen, tools should be hidden after couple of time.
    Tool bar's position is too high
  Features:
    To open a file, shortcut "CTRL + O" will be added.

Current state
A video can be opened from menubar -> File -> Open.
Change the playback rate from buttons next to soundbar.

Shortcuts:
  space = pause
  Escape = exit fullscreen
  Left = 5 seconds backward
  Right = 5 seconds forward
  Up = %5 volume up
  Down = %5 volume down
  + = increase playback rate by 0.25
  - = decrease playback rate by 0.25
  R = reset playback rate to 1.0
  F = toggle fullscreen
  M = toggle mute
  J = 10 seconds backward
  K = pause
  L = 10 seconds forward
  numbers 0 to 9 = seek to %number of video -> if you press 4, it will go to %40 of video

Bugs I found:
  Let's say a video is opened and frames are loading for hover. And you opened new video. Before new video's entire frames loaded, if you move your mouse to slide bar, you will see the old video's frames instead of new video. And new frames will not be loaded.
