import pygame as pygame
from pygame import Surface, image as image
from pygame.locals import *
import glob
import random
import sys


class slide:
    # connect speed to time
    def __init__(self, image: Surface, height, speedX: float, speedY: float) -> None:
        self.speedX = speedX
        self.speedY = speedY
        self.image = image
        self.pos = image.get_rect()
        self.internal_pos = [*self.pos.topleft]

    def setImage(self, image: Surface) -> None:
        self.image = image
        self.pos = image.get_rect()

    def setSpeed(self, speedX, speedY) -> None:
        self.speedX = speedX
        self.speedY = speedY

    def setpos(self, x: float, y: float) -> None:
        self.internal_pos[0] = x
        self.internal_pos[1] = y

    def move(self) -> None:  # move x y
        # stop option
        # if self.internal_pos[0] <-400 or self.internal_pos[0] >400:
        #     self.internal_pos[0] = 0
        self.pos.x = self.internal_pos[0]
        self.internal_pos[0] += self.speedX
        self.pos.y = self.internal_pos[1]
        self.internal_pos[1] += self.speedY


class JPGList:
    def __init__(self, path, output) -> None:
        self.path = path
        self.output = output
        self.file_amount = 0
        self.listJPGFiles(path, output)

    def listJPGFiles(self, path: str, output: str) -> bool:
        output_file = open(output, 'w')        
        filenames = glob.glob(path+'/**/*.jpg',recursive=True)
        for file in filenames:
            output_file.write(file+'\n')
        self.file_amount = len(filenames)
        output_file.close()

    def getRandomPic(self, rand=-1) -> str:
        if rand == -1:
            rand = random.randint(0, self.file_amount-1)
        with open(self.output) as output_file:
            for i, line in enumerate(output_file):
                if i == rand:
                    return line
        raise("ERROR: cant find the line")

    def loadImage(self, file_path: str) -> Surface:
        raise("loadImage not implemented yet")

    def refresh(self):
        raise("refresh not implemented yet")


frame_time = 2  # seconds
# get w and h of full screen
screen = pygame.display.set_mode((640, 480))

jpg_list = JPGList("./pics", "test.txt")

front = pygame.image.load(jpg_list.getRandomPic().strip('\n'))
screen.fill("black")

forground = slide(front, 0, 0.1, -0.1)
background = slide(front, 0, 0, 0)
time = pygame.time.get_ticks()

while 1:
    for event in pygame.event.get():
        if event.type in (QUIT, KEYDOWN):
            sys.exit()
    print(pygame.time.get_ticks())
    if pygame.time.get_ticks()-time > frame_time*1000:
        time = pygame.time.get_ticks()
        forground.setImage(pygame.image.load(
            jpg_list.getRandomPic().strip('\n')))
        background.setImage(forground.image)
        forground.setpos(0, 0)
        forground.setSpeed((random.random()-0.5)/2, (random.random()-0.5)/2)
        background.setpos(0, 0)

    screen.fill("black")
    screen.blit(background.image, background.pos)
    screen.blit(forground.image, forground.pos)
    forground.move()
    pygame.display.update()
    pygame.time.delay(3)
