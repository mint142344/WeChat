## WeChat-Server

## GateServer
 Client -> Listener -> HttpConnection -> LogicSystem -> RouteHandler -> Response

 `Listener`为每个tcp连接创建一个`HttpConnection`, 接收请求后由单例类
 `LogicSystem`根据route设置响应，由`HttpConnection`发送回去

## ConfigManager
 单例实现配置文件管理类
 1. 配置`GateServer`ip和port
 2. 配置 RPC 邮箱服务host和port

## 邮箱服务
- RPC客户端

 `GateServer`搭建邮箱验证 RPC 服务客户端(单例)
- RPC服务端

 Nodejs实现rpc server，完成邮箱服务
 
 `npm install @grpc/grp-js @grpc/proto-loader nodemailer ioredis`