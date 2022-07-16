# WeatherAppGUI

WeatherAppGUI was create using C++ language. You can type a city which weather
you want to see.

## Libraries and API's which was used

GUI: FLTK[^1]
JSON Parser: cJSON[^2]
WeatherAPI: OpenWeatherApp API[^3]

## How to build

Type in terminal:

```
make WeatherApp
```

> Note: you should have installed libraries FLTK in your PC. It will not a
> problem with cJSON because in my source files there are header file `cJSON.h`
> and `cJSON.c`

## How to run

To run a programm type in terminal:

```
./WeatherApp
```

Type in a search input city about which information you want to get and you get
a result. If you wrote wrong city or it was a problem with request you see
a message box.

### From author

I know that programm's UI is not good, but my purpose in programm was to show 
how to get data via network using API's. Of course you can take the initiative 
and write a good UI not even on FLTK.

[^1]: Link: https://www.fltk.org/
[^2]: Link: https://github.com/DaveGamble/cJSON
[^3]: Link: https://openweathermap.org/api