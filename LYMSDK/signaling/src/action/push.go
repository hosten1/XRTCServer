package action

import (
	// "fmt"
	// "html/template"
	"test/src/framework"
	"net/http"
)

type PushAction struct{

}
func NewPushAction()  *PushAction {
		return &PushAction{}
}

func (*PushAction) Execute(w http.ResponseWriter, cr *framework.CommonRequest){

}