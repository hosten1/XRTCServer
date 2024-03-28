package action

import (
	"fmt"
	"html/template"
	"test/src/framework"
	"net/http"
)

type lrtcClientPushAction struct{}

func NewLrtcClientPushAction() *lrtcClientPushAction {
	return &lrtcClientPushAction{}
}
func WriteHtmlErrorResopnse(w http.ResponseWriter,status int ,err string){
	w.WriteHeader(status)
	w.Write([]byte(fmt.Sprintf("%d==%s", status, err)))
}
func (a *lrtcClientPushAction) Execute(w http.ResponseWriter, r *framework.CommonRequest) {
	// fmt.Println("Hi!! test l rtc client")
  r := r.R
  t,err :=template.ParseFiles("./static/template/push.html")
   if err != nil {
	fmt.Println(err)
	WriteHtmlErrorResopnse(w,http.StatusNotFound,"404 - Not Found")
	return
   }
   request := make(map[string]string )
   for k,v := range r.Form  {
	   request[k] = v[0]
   }	
   err = t.Execute(w,request)
   if err != nil {
		fmt.Println(err)
		WriteHtmlErrorResopnse(w,http.StatusNotFound,"404 - Not Found")
   }
}
