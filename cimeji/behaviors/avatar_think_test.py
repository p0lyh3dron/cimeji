import cimeji

import random
from enum import Enum, auto
from typing import Dict, List, Tuple


FRAMES_WALK = [
    "shime1.png",
    "shime2.png",
    "shime1.png",
    "shime3.png",
]

FRAMES_GRABBED = [
    "shime5.png",
    "shime5b.png",
]

FRAMES_FALL = [
    "shime4.png",
]

FRAMES_GET_UP = [
    "shime18.png",
    "shime19.png",
    "shime30.png",
]

FRAMES_IDLE = [
    "shime1.png",
    "shime1w1.png",
]


# TODO: come up with an actual flexible ai system
#  will have to do a good bit of research, parts of this will probably be used for sidury

# maybe split ai and animation into separate files?
# yeah probably do that

# behavior nodes:
# - look for window
# - stand and blink
# - walk/run in a direction
# - walk/run to a target
# - climb a wall
# - jump to a wall
# - jump onto a window
# - sit down
# - sit down + face cursor
# - sit down and tilt head back and forth
# - falling
# - crawling on ground
# - create new mascot (if allowed)
#   - split into two
#   - or pull one from ground


# node types:
# - entry node (probably not)
# - action node
# - condition node


class Node:
    def __init__(self):
        pass


class Action(Node):
    def __init__(self):
        super().__init__()


class Condition(Node):
    def __init__(self):
        super().__init__()

    def check(self) -> bool:
        pass


class ActionType(Enum):
    Stay = auto(),
    Move = auto(),
    Animate = auto(),
    Select = auto(),  # ok i have no idea lol
    Embedded = auto(),  # wtf does that mean, maybe doing this with whatever is controlling it?
                        # like attatched to a window or grabbed on the cursor?


class BorderType(Enum):
    Floor = auto(),
    Ceiling = auto(),
    Wall = auto(),


# also check if it loops


class BaseAction:
    def __init__(self, name: str, manager: "Manager", avatar):
        self.name: str = name
        self.manager: Manager = manager
        self.avatar = avatar

    # setup any conditions and other stuff after creating actions
    def init(self, dt: float):
        pass

    # change to this action
    def on_set(self, dt: float):
        pass

    # leaving this action
    def on_change(self, dt: float, new_action: "BaseAction"):
        pass

    # main think function
    def think(self, dt: float):
        pass

    # condition checking
    def pick_action(self, dt: float) -> "BaseAction":
        return self


class IdleAction(BaseAction):
    def __init__(self, manager, avatar):
        super().__init__("idle", manager, avatar)

        self.blinkFrame = 0
        self.blinkTime = 0

        self.walkTime = -1

    def on_set(self, dt: float):
        self.blinkFrame = 0
        self.blinkTime = self.manager.time
        self.walkTime = -1

    # condition checking
    def pick_action(self, dt: float):
        if self.walkTime == -1:
            self.walkTime = self.manager.time + random.randint(5, 15)

        if self.walkTime <= self.manager.time:
            return self.manager.get_action("walk", self)

        return self

    def think(self, dt: float):
        if self.blinkTime <= self.manager.time:
            cimeji.set_avatar_image(self.avatar, FRAMES_IDLE[self.blinkFrame])

            self.blinkFrame += 1
            if self.blinkFrame == len(FRAMES_IDLE):
                self.blinkFrame = 0
                self.blinkTime += random.randint(3, 6) / 10
            elif self.blinkFrame == 1:
                self.blinkTime += random.randint(3, 10)


class WalkAction(BaseAction):
    def __init__(self, manager, avatar):
        super().__init__("walk", manager, avatar)

        self.walkTime = 0
        self.walkFrame = 0
        self.walkPosTime = 0

    def on_set(self, dt: float):
        self.walkFrame = 0
        self.walkTime = self.manager.time
        self.walkPosTime = self.manager.time

    def think(self, dt: float):
        pos_x, pos_y = cimeji.get_avatar_pos(self.avatar)

        if self.walkPosTime <= self.manager.time:
            cimeji.set_avatar_pos(self.avatar, pos_x - 1, pos_y)
            self.walkPosTime += 0.01

        if self.walkTime <= self.manager.time:
            cimeji.set_avatar_image(self.avatar, FRAMES_WALK[self.walkFrame])

            self.walkFrame += 1
            if self.walkFrame == len(FRAMES_WALK):
                self.walkFrame = 0

            self.walkTime += 0.2


class GrabbedAction(BaseAction):
    def __init__(self, manager, avatar):
        super().__init__("grabbed", manager, avatar)

        self.grabbedTime = 0
        self.grabbedFrame = 0

    def on_set(self, dt: float):
        self.grabbedFrame = 0
        self.grabbedTime = self.manager.time

    def pick_action(self, dt: float) -> BaseAction:
        if cimeji.avatar_grabbed(self.avatar):
            return self

        return self.manager.get_action("fall", self)

    def think(self, dt: float):
        if self.grabbedTime <= self.manager.time:
            cimeji.set_avatar_image(self.avatar, FRAMES_GRABBED[self.grabbedFrame])

            self.grabbedFrame += 1
            if self.grabbedFrame == len(FRAMES_GRABBED):
                self.grabbedFrame = 0

            self.grabbedTime += 0.1


class FallAction(BaseAction):
    def __init__(self, manager, avatar):
        super().__init__("fall", manager, avatar)

        self.time = 0
        self.frameTime = 0
        self.frame = 0

    def on_set(self, dt: float):
        self.frame = 0
        self.time = self.manager.time

    def pick_action(self, dt: float) -> BaseAction:
        # TODO: set this up, and if it finds a window, see what direction it is and do it accordingly
        # if not cimeji.env_find_window(self.avatar):
        #     return self

        return self.manager.get_action("get_up", self)

    def think(self, dt: float):
        pos_x, pos_y = cimeji.get_avatar_pos(self.avatar)

        if self.time <= self.manager.time:
            cimeji.set_avatar_pos(
                self.avatar,
                pos_x + self.manager.gravity[0],
                pos_y + self.manager.gravity[1]
            )
            self.time += 0.01

        if self.frameTime <= self.manager.time:
            cimeji.set_avatar_image(self.avatar, FRAMES_FALL[self.frame])

            self.frame += 1
            if self.frame == len(FRAMES_FALL):
                self.frame = 0

            self.frameTime += 0.1


# TODO: really should create some base action types to use
# like a simple animation playing action
class GetUpAction(BaseAction):
    def __init__(self, manager, avatar):
        super().__init__("get_up", manager, avatar)

        self.finished = False
        self.frameTime = 0
        self.frame = 0

    def on_set(self, dt: float):
        self.frame = 0
        self.frameTime = self.manager.time
        self.finished = False

    def pick_action(self, dt: float) -> BaseAction:
        if self.finished:
            return self.manager.get_action("idle")

        return self

    def think(self, dt: float):
        if self.finished:
            return

        if self.frameTime <= self.manager.time:
            if self.frame + 1 == len(FRAMES_GET_UP) + 1:
                self.finished = True
                return

            cimeji.set_avatar_image(self.avatar, FRAMES_GET_UP[self.frame])

            self.frame += 1
            self.frameTime += 0.6


class Manager:
    def __init__(self, avatar):
        self.avatar = avatar
        self.time: float = 0

        # self.tick = 0
        # self.tickRate = 20

        self.attached = True
        self.velocity: Tuple[float, float] = (0, 0)
        self.gravity: Tuple[float, float] = (0, 2)

        self.action: BaseAction = None
        self._actions: Dict[str, BaseAction] = {}

    def init_frames(self):
        pass

    def init_default_actions(self):
        self.register_action(IdleAction(self, self.avatar))
        self.register_action(WalkAction(self, self.avatar))
        self.register_action(FallAction(self, self.avatar))
        self.register_action(GrabbedAction(self, self.avatar))
        self.register_action(GetUpAction(self, self.avatar))

        self.action = self._actions["idle"]

    def register_action(self, action: BaseAction):
        self._actions[action.name] = action

    def get_action(self, name: str, fallback: BaseAction = None) -> BaseAction:
        if name in self._actions:
            return self._actions[name]

        print(f"[Manager] Warning: action does not exist: \"{name}\"")
        if fallback:
            return fallback

        return self._actions["idle"]

    def pick_action(self, dt: float) -> BaseAction:
        if cimeji.avatar_grabbed(self.avatar):
            return self.get_action("grabbed")

        # check in air and not attached to anything
        if not self.attached:
            # check if in air
            # return self.get_action("fall")
            pass

        return self.action.pick_action(dt)

    def think(self, dt: float):
        self.time += dt

        # TODO: implement some kind of basic physics system thing here
        #  at least a concept of velocity, friction, and gravity for being tossed and when in the air

        new_action = self.pick_action(dt)
        if self.action != new_action:
            print(f"[Manager] Switching Action: \"{self.action.name if self.action else ''}\" -> \"{new_action.name}\"")
            self.action.on_change(dt, new_action)
            self.action = new_action
            self.action.on_set(dt)

        self.action.think(dt)


g_avatars: Dict[float, Manager] = {}


def think(avatar, dt: float):
    if avatar not in g_avatars:
        g_avatars[avatar] = Manager(avatar)
        g_avatars[avatar].init_default_actions()
        g_avatars[avatar].init_frames()

    g_avatars[avatar].think(dt)

