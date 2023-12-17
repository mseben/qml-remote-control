### run
```
cmake -B build
cmake --build build/
LD_PRELOAD=build/libqmlremotecontrol.so build/qmltestapp/qmltestapp
```
### test
```
python3 -m websockets ws://localhost:2709/
Connected to ws://localhost:2709/.
> button.click
```
