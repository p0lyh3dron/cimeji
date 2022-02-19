import cimeji

import random
from enum import Enum, auto
from typing import Dict, List


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

FRAMES_IDLE = [
    "shime1.png",
    "shime1w1.png",
]


# TODO: come up with an actual flexible ai system
#  will have to do a good bit of research, parts of this will probably be used for sidury


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
    def __init__(self, manager, avatar):
        self.manager: Manager = manager
        self.avatar = avatar

    # change to this behavior
    def on_set(self, dt: float):
        pass

    # change to a new behavior
    def on_change(self, dt: float):
        pass

    # main think function
    def think(self, dt: float):
        pass


class IdleAction(BaseAction):
    def __init__(self, manager, avatar):
        super().__init__(manager, avatar)

        self.blinkFrame = 0
        self.blinkTime = 0

    def on_set(self, dt: float):
        self.blinkFrame = 0
        self.blinkTime = self.manager.time

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
        super().__init__(manager, avatar)

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
        super().__init__(manager, avatar)

        self.grabbedTime = 0
        self.grabbedFrame = 0

    def on_set(self, dt: float):
        self.grabbedFrame = 0
        self.grabbedTime = self.manager.time

    def think(self, dt: float):
        if self.grabbedTime <= self.manager.time:
            cimeji.set_avatar_image(self.avatar, FRAMES_GRABBED[self.grabbedFrame])

            self.grabbedFrame += 1
            if self.grabbedFrame == len(FRAMES_GRABBED):
                self.grabbedFrame = 0

            self.grabbedTime += 0.1


class Manager:
    def __init__(self, avatar):
        self.avatar = avatar
        self.time = 0
        self.action: BaseAction = None
        self.actions: List[BaseAction] = []

        self.grabbedAction = None

    def init_default_actions(self):
        self.actions.append(IdleAction(self, self.avatar))
        self.actions.append(WalkAction(self, self.avatar))

        self.grabbedAction = GrabbedAction(self, self.avatar)

    def register_action(self, action: BaseAction):
        pass

    def pick_action(self, dt: float) -> BaseAction:
        if cimeji.avatar_grabbed(self.avatar) and self.grabbedAction is not None:
            if self.action != self.grabbedAction:
                self.action = self.grabbedAction
                self.action.on_set(dt)
            return self.action

        return self.actions[0]

    def think(self, dt: float):
        self.time += dt

        # TODO: implement some kind of basic physics system thing here
        #  at least a concept of velocity, friction, and gravity for being tossed and when in the air

        newAction = self.pick_action(dt)
        if self.action != newAction:
            self.action = newAction
            self.action.on_set(dt)

        self.action.think(dt)


g_avatars: Dict[float, Manager] = {}


def think(avatar, dt: float):
    if avatar not in g_avatars:
        g_avatars[avatar] = Manager(avatar)
        g_avatars[avatar].init_default_actions()

    g_avatars[avatar].think(dt)

