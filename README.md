# A Simple C/C++ Based HTTP File Server
---
## Introduction

A very simple file server that everyone can download data from a specified(shared) folder. This project helps users figure 
out how http servers work and how they are build.

## Features
- Static webpage to list files
![demo1.png](https://github.com/lanlanblue/http-server/blob/master/demo1.png)
- Build connection through socket
  1. Create a socket
  2. Identify(Name) a socket
  3. On the server, wait for an incoming connection (Listen)
  4. Send & receive messages
- Process GET requests for requesting directories/files
```
GET /pictures/dogs HTTP/1.1
Host: 0.0.0.0:8080
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.100 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3
Referer: http://0.0.0.0:8080/pictures
Accept-Encoding: gzip, deflate
Accept-Language: zh-TW,zh;q=0.9,en-US;q=0.8,en;q=0.7,zh-CN;q=0.6,de;q=0.5
If-Modified-Since: Wed, 22 Jul 2009 19:15:56 GM
```
- Reply 2 kinds of responses
  - Directory contents
  ![demo2.png](https://github.com/lanlanblue/http-server/blob/master/demo2.png)
  - Download response
  ![demo3.png](https://github.com/lanlanblue/http-server/blob/master/demo3.png)

