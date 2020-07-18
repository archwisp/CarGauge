### Install Platformio
```
python2.7 -m pip install platformio
```

### Update espressif32
```
cd ~/.platformio/packages/framework-arduinoespressif32/tools
./get.py
```

### Build
```
python2.7 -m platformio run
```

#### Clean build
```
python2.7 -m platformio run -t clean
```

### Deploy
```
python2.7 -m platformio run -t upload
```
