package framework

import (
	"fmt"
	"net/http"
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
	Logger  commonLog
	LogId   uint32
}
func getRealClientIP(r *http.Request) string{
   ip := r.RemoteAddr
   if rip := r.Header.Get("X-Real-IP"); len(rip) > 0 {
       ip = rip
   } else if rip :=  r.Header.Get("X-Forwarded-IP"); len(rip) > 0 {
       ip = rip
   }
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
	// 查找路由处理器
	if action, ok := GActionRouter[r.URL.Path]; ok {
		if action != nil {
			cr := &CommonRequest{
				R: r,
				Logger: commonLog{},
				LogId: getLogId32(),
			}
			cr.Logger.AddNotice("log_id", strconv.Itoa(int(cr.LogId)))
			cr.Logger.AddNotice("url", r.URL.Path)
			cr.Logger.AddNotice("referer", r.Header.get("Referer"))
			cr.Logger.AddNotice("user_agent", r.Header.get("User-Agent"))
			cr.Logger.AddNotice("clientIp", r.RemoteAddr)
			cr.Logger.AddNotice("cookit",r.Header.get("Cookie"))
			cr.Logger.AddNotice("clientRealIP",getRealClientIP(r))
			r.ParseForm()//解析url中参数
			action.Execute(w, cr)
			cr.Logger.Infof("http request uri = " + r.URL.Path)
		} else {
			responseError(w, http.StatusInternalServerError, "Internal server error")
		}
	} else {
		responseError(w, http.StatusNotFound, "Not found")
	}
}
// 启动 HTTP 服务器
func StartHttp() error {
	fmt.Println("start http!!!")
	return http.ListenAndServe(":8080", nil)
}