# Bindings for use in something like pycharm or vscode
# also required to import so

from typing import Tuple

'''
    std::vector< shimeji_data_t* > aData;
    u16              aPos[ 2 ];
    behavior_t*      apBehavior = nullptr;
    u16              aWidth;
    u16              aHeight;
    u16              aFrame;
    bool             aGrabbed = false;
'''


class avatar_t:
    def __init__(self):
        self.aData = [],
        self.aSizeX = 0,
        self.aSizeY = 0,


class monitor_t:
    def __init__(self):
        self.aId = 0,
        self.aSizeX = 0,
        self.aSizeY = 0,


def get_avatar_pos(avatar) -> Tuple[int, int]:
    pass


def set_avatar_pos(avatar, x: int, y: int):
    pass


def set_avatar_image(avatar, image: str):
    pass


def avatar_grabbed(avatar) -> bool:
    pass


# ===========================================================================
# Desktop Environment Functions


def env_find_window(avatar):
    pass

