# imports here
import time
from controller import Controller
from machine import Pin
led = Pin(25, Pin.OUT)
led.toggle()

print("-- Minigotchi by dj1ch --")
print("\nYou can turn off the light on the pico by running 'led.toggle()' in the terminal here.")
