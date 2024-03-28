package framework

import (
	"fmt"
	"net/http"
)

// 定义路由处理接口
type ActionInterface interface {
	Execute(w http.ResponseWriter, r *http.Request)
}

// 路由处理器映射表
var GActionRouter map[string]ActionInterface = make(map[string]ActionInterface)

// 注册路由处理器
func init() {
	http.HandleFunc("/", entry)
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

	fmt.Println(">>>>>", r.URL.Path)

	// 查找路由处理器
	if action, ok := GActionRouter[r.URL.Path]; ok {
		r.ParseForm()//解析url中参数
		if action != nil {
			action.Execute(w, r)
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