# libuv_webserver

libuv 1.x  compatiable  
only test on mac

## set up  enviroment
```
brew install libuv
```
## build & run
```
make && ./webserver
```

## clean
```
make clean
```

## bench
Not bad   
```
ab -t 30 -c 100 http://127.0.0.1:8000/
```
![bench](https://github.com/zk4/libuv_webserver/blob/master/bench.jpg?raw=true)  

  
Instruction video by Ryan Dahl can be seen in http://vimeo.com/24713213)

 
