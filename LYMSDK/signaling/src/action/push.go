package action

import (
	"fmt"
	"strconv"
	"test/src/framework"
	"test/src/commonErrors"
	"net/http"
)

type PushAction struct{

}
func NewPushAction()  *PushAction {
		return &PushAction{}
}

func (*PushAction) Execute(w http.ResponseWriter, cr *framework.CommonRequest){
  r := cr.R
  //uid
  var strUid string
  if values , ok := r.Form["uid"]; ok {
	strUid = values[0]
  }
  uid,err := strconv.ParseUint(strUid,10,64)
  if err != nil || uid <= 0 {
	cerr := commonErrors.New(commonErrors.ParamErr,"parse uid error:"+err.Error())
	writeJsonErrorResponse(cerr,w,cr)
	return
	
  }

  //streamName
  var streamName string
  if values , ok := r.Form["streamName"]; ok {
	streamName = values[0]
  }
  if "" == streamName {
	cerr := commonErrors.New(commonErrors.ParamErr," streamName is null!!")
	writeJsonErrorResponse(cerr,w,cr)
	return
  }

  //audio
  var audioStr string
  var audio int
  if values , ok := r.Form["audio"]; ok {
	audioStr = values[0]
  }
  if "" == audioStr || "0" == audioStr {
	audio = 0;
  }else{
	audio = 1;
  }

  //streamName
  var videoStr string
  var video int
  if values , ok := r.Form["video"]; ok {
	videoStr = values[0]
  }
  if "" == videoStr || "0" == videoStr {
	video = 0;
  }else{
	video = 1;
  }
  fmt.Println(uid,streamName,audio,video)
}