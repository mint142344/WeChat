## WeChat-Server

## GateServer
 Client -> Listener -> HttpConnection -> LogicSystem -> RouteHandler -> Response

 `Listener`为每个tcp连接创建一个`HttpConnection`, 接收请求后由单例类
 `LogicSystem`根据route设置响应，由`HttpConnection`发送回去


## 邮箱服务
 Nodejs邮箱服务，服务之间通信gRPC