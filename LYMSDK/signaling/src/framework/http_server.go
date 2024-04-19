package framework

import (
	"fmt"
	"net/http"
	"strconv" // 导入 strconv 包
	"test/src/glog"
	"os"
)

// 定义路由处理接口
type ActionInterface interface {
	Execute(w http.ResponseWriter, r *CommonRequest)
}

// 路由处理器映射表
var GActionRouter map[string]ActionInterface = make(map[string]ActionInterface)

// 注册路由处理器
func init() {
	http.HandleFunc("/", entry)
}

 type CommonRequest struct {
	R *http.Request
	Logger  CommonLog
	LogId   uint32
}
func GetRealClientIP(r *http.Request) string{
   ip := r.RemoteAddr
   if rip := r.Header.Get("X-Real-IP"); len(rip) > 0 {
       ip = rip
   } else if rip :=  r.Header.Get("X-Forwarded-IP"); len(rip) > 0 {
       ip = rip
   }
   return ip
}
// 响应错误
func responseError(w http.ResponseWriter, status int, err string) {
	w.WriteHeader(status)
	w.Write([]byte(fmt.Sprintf("%d==%s", status, err)))
}

// 请求入口函数
func entry(w http.ResponseWriter, r *http.Request) {
	// 处理浏览器请求图标的情况
	if "/favicon.ico" == r.URL.Path {
		w.WriteHeader(http.StatusOK)
		w.Write([]byte(""))
		return
	}
	glog.Infof("-----> entry :%s ",r.URL.Path)

	// 查找路由处理器
	if action, ok := GActionRouter[r.URL.Path]; ok {
		if action != nil {
			cr := &CommonRequest{
				R: r,
				Logger: CommonLog{},
				LogId: getLogId32(),
			}

			cr.Logger.AddNotice("log_id", strconv.Itoa(int(cr.LogId)))
			cr.Logger.AddNotice("url", r.URL.Path)
			cr.Logger.AddNotice("referer", r.Header.Get("Referer"))
			cr.Logger.AddNotice("user_agent", r.Header.Get("User-Agent"))
			cr.Logger.AddNotice("clientIp", r.RemoteAddr)
			cr.Logger.AddNotice("cookit",r.Header.Get("Cookie"))
			cr.Logger.AddNotice("clientRealIP",GetRealClientIP(r))
			r.ParseForm()//解析url中参数

			for k , v := range r.Form {
				cr.Logger.AddNotice(k,v[0])
				
			}
            cr.Logger.TimeBegin("timeCast")
			action.Execute(w, cr)
			cr.Logger.TimeEnd("timeCast")
			cr.Logger.Infof(" ")
		} else {
			responseError(w, http.StatusInternalServerError, "Internal server error")
		}
	} else {
		responseError(w, http.StatusNotFound, "Not found")
	}
}
func RegisterStaticUrl(){
	staticDir := gconf.httpStaticDic // 静态文件目录路径

	// 检查静态文件目录是否存在
	if _, err := os.Stat(staticDir); os.IsNotExist(err) {
		glog.Infof("静态文件目录 %s 不存在\n", staticDir)
		return
	}

	// 检查静态文件目录访问权限
	if _, err := os.Stat(staticDir); err != nil {
		glog.Infof("无法访问静态文件目录 %s：%v\n", staticDir, err)
		return
	}
	glog.Infof("start http static file server on dic path :%s prifx:%s",staticDir,gconf.httpStaticPrefix)

	fs := http.FileServer(http.Dir(staticDir))
	http.Handle(gconf.httpStaticPrefix,http.StripPrefix(gconf.httpStaticPrefix,fs))
}
// 启动 HTTP 服务器
func StartHttp() error {
	glog.Infof("start http server on port :%d",gconf.httpPort)
	return http.ListenAndServe(fmt.Sprintf(":%d",gconf.httpPort), nil)
}
// 启动 HTTP 服务器
func StartHttps() error {
	// 指定证书文件和私钥文件
	certFile := gconf.httpsCert
	keyFile := gconf.httpsKey
	glog.Infof("start https server on port :%d",gconf.httpsPort)
	return http.ListenAndServeTLS(fmt.Sprintf(":%d",gconf.httpsPort), certFile, keyFile, nil)
}