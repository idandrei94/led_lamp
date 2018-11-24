#!/usr/bin/env python3

# NOTE: this example requires PyAudio because it uses the Microphone class

import serial
import speech_recognition as sr
import time
import os

pidfile = open('/home/pi/Desktop/LEDLamps/pid', 'w+')
pidfile.write(str(os.getpid()))
pidfile.close()

NIGHT_MODE = {
  "make it shine",
  "let it shine",
  "night mode",
  "enable night mode",
  "light please",
  "lights please",
  "lights on",
  "light on"
  }

DIM = {
  "light off",
  "lights off",
  "dim lights"
}

SINGLE_FENIX = {
  "single fenix mode",
  "single fenix",
  "enable single fenix mode",
  "enable single fenix"
  "single phoenix mode",
  "single phoenix",
  "enable single phoenix mode",
  "enable single phoenix",
  "enable phoenix",
  "phoenix mode",
  "enable animation one",
  "enable animation 1",
  "animation one",
  "animation 1"
}

DOUBLE_FENIX = {
  "double fenix mode",
  "double fenix",
  "enable double fenix mode",
  "enable double fenix"
  "double phoenix mode",
  "double phoenix",
  "enable double phoenix mode",
  "enable double phoenix",
  "enable animation two",
  "enable animation 2",
  "enable animation to",
  "animation 2",
  "animation two",
  "animation to"
}

SINGLE_GREEN = {
  "single green mode",
  "single green",
  "enable single green mode",
  "enable single green",
  "enable green mode",
  "enable animation three",
  "enable animation tree",
  "enable animation 3",
  "enable animation free",
  "animation three",
  "animation tree",
  "animation 3",
  "animation free"
}

DOUBLE_GREEN = {
  "double green mode",
  "double green",
  "enable double green mode",
  "enable double green",
  "enable animation four",
  "enable animation 4",
  "enable animation for",
  "animation four",
  "animation 4",
  "animation for"
}

WHITE = {
  "enable daylight",
  "enable day light",
  "daylight",
  "day light"
}

def handle_speech(str, s):
    if str in NIGHT_MODE :
        print("lights turning on")
        s.write(bytearray('0','ascii'))
    elif str in DIM :
        print("lights turning off" )
        s.write(bytearray('9','ascii'))
    elif str in SINGLE_FENIX:
        print("starting single fenix" )
        s.write(bytearray('5','ascii'))
    elif str in DOUBLE_FENIX:
        print("starting double fenix" )
        s.write(bytearray('6','ascii'))
    elif str in SINGLE_GREEN:
        print("starting single green" )
        s.write(bytearray('3','ascii'))
    elif str in DOUBLE_GREEN:
        print("starting double green" )
        s.write(bytearray('4','ascii'))
    elif str in WHITE:
        print("turning day light")
        s.write(bytearray('1', 'ascii'))

# obtain audio from the microphone
r = sr.Recognizer()
r.dynamic_energy_threshold = True

print("Initiating serial device")
try:
  s = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=9600,
    dsrdtr=True,
    bytesize=serial.EIGHTBITS,
    timeout=1)
  s.close()
  s.open()
  time.sleep(3)
except serial.SerialException:
        print("Cannot open serial port")
        exit()
print("Jacked up and ready to go")

while True:
  with sr.Microphone() as source:
    print('Calibrating microphone...')
    r.adjust_for_ambient_noise(source, duration=1)
    print("Say something!")
    audio = r.listen(source, phrase_time_limit=3)
  print("Processing...")
# recognize speech using Google Speech Recognition
  try:
    # for testing purposes, we're just using the default API key
    # to use another API key, use `r.recognize_google(audio, key="GOOGLE_SPEECH_RECOGNITION_API_KEY")`
    # instead of `r.recognize_google(audio)`
    speech = r.recognize_google(audio).lower()
    print("Google Speech Recognition thinks you said " + speech)
    handle_speech(speech,s)
  except sr.UnknownValueError:
    print("Google Speech Recognition could not understand audio")
  except sr.RequestError as e:
    print("Could not request results from Google Speech Recognition service; {0}".format(e))
