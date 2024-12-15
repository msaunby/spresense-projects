# spresense-projects

## WiFi

https://developer.sony.com/spresense/tutorials-sample-projects/spresense-tutorials/how-to-equip-spresense-with-wifi-connectivity

Example uses UART2_TX, UART2_RX, 3V3 and GND, all from main board (not extension).  
These are D00 and D01 on extension board.

For deployment with doppler radar, one device will need software serial.
https://developer.sony.com/spresense/spresense-api-references-arduino/classSoftwareSerial.html

See https://github.com/kamtom480/ITEADLIB_Arduino_WeeESP8266 for how to change.
 
### POST

Assume image is approx 50 k bytes.  These should transfer fine as a single POST.

Use base64 encode. See https://stackoverflow.com/questions/29104107/upload-image-using-post-form-data-in-python-requests

https://github.com/Densaugeo/base64_arduino


Something like
```
POST /update HTTP/1.1
Host: api.thingspeak.com
{api_key: "api", field1: 30}
Content-Type: application/json
```

## Resources

https://gpsd.gitlab.io/gpsd/ppp-howto.html

https://developer.sony.com/develop/spresense/developer-tools/api-reference/api-references-arduino/classSpNavData.html#details

https://files.igs.org/pub/data/format/rinex305.pdf

https://en.wikipedia.org/wiki/Dilution_of_precision_(navigation)

https://www.denshi.e.kaiyodai.ac.jp/gnss_tutor/base_station.html



## Plotting

https://pypi.org/project/jupyter-scatter/
