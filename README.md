# HoverVideoPlayer

It's a basic video player that focuses on the **hover preview feature** (like YouTube). When you move your cursor over the seek bar, it shows the specific frame at that position.

> **Note:** The main focus of this project is the *hover feature*. It is not intended to replace feature-rich renderers like VLC.

## 🎮 Usage
* **Open File:** Currently via `Menu Bar -> File -> Open`.
* **Playback Rate:** Can be changed using the buttons next to the soundbar.

## ⌨️ Shortcuts

| Key | Action |
| :--- | :--- |
| `Space` / `K` | Play / Pause |
| `F` | Toggle Fullscreen |
| `Esc` | Exit Fullscreen |
| `Left` / `Right` | Seek 5 seconds backward / forward |
| `J` / `L` | Seek 10 seconds backward / forward |
| `Up` / `Down` | Volume Up / Down (%5) |
| `M` | Toggle Mute |
| `+` | Increase Playback Rate (+0.25) |
| `-` | Decrease Playback Rate (-0.25) |
| `R` | Reset Playback Rate to 1.0 |
| `0` - `9` | Seek to %0 - %90 of the video (e.g. 4 -> %40) |

## 🐛 Known Bugs
- [x] **Frame Loading Conflict:** If a video is opened and its frames are still loading for the hover preview, and you immediately open a *new* video; the slider might display the **old video's frames** instead of the new ones, and the new frames may fail to load.
- [ ] **Canceling an opening progress:** When a video is open, if you follow the *File -> Open* path and press cancel or close tab without opening new video, rest frames are not being loaded.

## 📝 To-Do (Planned Changes)
### Design
- [ ] In fullscreen mode, toolbars should auto-hide after a few seconds.
- [ ] Fix the toolbar position (currently too high).

### Features
- [ ] Add `Ctrl + O` shortcut to open files.
