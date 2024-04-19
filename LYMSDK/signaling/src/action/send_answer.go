package action

import (
	"fmt"
	"strconv"
  "encoding/json"
	"test/src/framework"
	"test/src/commonErrors"
	"net/http"
)

type sendAnswerAction struct{

}
func NewSendAnswerAction()  *sendAnswerAction {
		return &sendAnswerAction{}
}

type lRtcSendAnswerhReq struct {
   Cmdno int `json:"cmdno"`
   Uid uint64 `json:"uid"`
   StreamName string `json:"stream_name"`
   Answer string `json:"answer"`
   Type string `json:"type"`
}
type lRtcSendAnswerResp struct {
  ErrorNo int `json:"err_no"`
  ErrorMsg string `json:"err_msg"`
}


func (*sendAnswerAction) Execute(w http.ResponseWriter, cr *framework.CommonRequest){
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
  
    //streamName
  var answerSdp string
  if values , ok := r.Form["answer"]; ok {
	answerSdp = values[0]
  }
  if "" == answerSdp {
	cerr := commonErrors.New(commonErrors.ParamErr," answerSdp is null!!")
	writeJsonErrorResponse(cerr,w,cr)
	return
  }
    //streamName
  var typeStr string
  if values , ok := r.Form["type"]; ok {
	typeStr = values[0]
  }
  if "" == typeStr {
	cerr := commonErrors.New(commonErrors.ParamErr," typeStr is null!!")
	writeJsonErrorResponse(cerr,w,cr)
	return
  }
  // fmt.Println(uid,streamName,audio,video)
  req := lRtcSendAnswerhReq{
    Cmdno : CMDNO_ANSWER,
    Uid :uid,
    StreamName:streamName,
    Answer:answerSdp,
    Type:typeStr,
  }
  var resp lRtcSendAnswerResp 
  err = framework.Call("lrtc",req,&resp,cr.LogId)
  // fmt.Printf( "Execute resp:%+v\n",resp)
  if err != nil {
    cerr := commonErrors.New(commonErrors.NetworkErr,"backend process error: " + err.Error())
    writeJsonErrorResponse(cerr,w,cr)
    return
  }
  if resp.ErrorNo != 0 {
    cerr := commonErrors.New(commonErrors.NetworkErr,fmt.Sprintf("backend process error: %d",resp.ErrorNo))
    writeJsonErrorResponse(cerr,w,cr)
    return
  } 
  httpResp := &CommonHttpResp{
    ErrNo: 0,
    ErrMsg: "success",
  }
  b,_ := json.Marshal(httpResp)
  cr.Logger.AddNotice("resp",string(b))
  w.Write(b)

}