# Update espressif32

cd ~/.platformio/packages/framework-arduinoespressif32/tools
./get.py

# Install Libraries

python2.7 -m platformio lib install "Adafruit GFX Library"
python2.7 -m platformio lib install "Adafruit ST7735 Library"
python2.7 -m platformio lib install https://github.com/adafruit/Adafruit-ST7735-Library
python2.7 -m platformio lib install https://github.com/stanleyhuangyc/ArduinoOBD
python2.7 -m platformio lib install "Adafruit NeoPixel"

# Build

python2.7 -m platformio run

# DEPLOY

python2.7 -m platformio run -t upload
