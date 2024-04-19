package action

import (
	"fmt"
	"strconv"
    "encoding/json"
	"test/src/framework"
	"test/src/commonErrors"
	"net/http"
)

type stopPullAction struct {
}

func NewStopPullAction() *stopPullAction {
	return &stopPullAction{}
}

type xrtcStopPullReq struct {
	Cmdno      int    `json:"cmdno"`
	Uid        uint64 `json:"uid"`
	StreamName string `json:"stream_name"`
}

type xrtcStopPullResp struct {
	ErrNo  int    `json:"err_no"`
	ErrMsg string `json:"err_msg"`
}

func (*stopPullAction) Execute(w http.ResponseWriter, cr *framework.CommonRequest) {
	r := cr.R

	// uid
	var strUid string
	if values, ok := r.Form["uid"]; ok {
		strUid = values[0]
	}

	uid, err := strconv.ParseUint(strUid, 10, 64)
	if err != nil || uid <= 0 {
		cerr := commonErrors.New(commonErrors.ParamErr, "parse uid error:"+err.Error())
		writeJsonErrorResponse(cerr, w, cr)
		return
	}

	//streamName
	var streamName string
	if values, ok := r.Form["streamName"]; ok {
		streamName = values[0]
	}

	if "" == streamName {
		cerr := commonErrors.New(commonErrors.ParamErr, "streamName is null")
		writeJsonErrorResponse(cerr, w, cr)
		return
	}

	req := xrtcStopPullReq{
		Cmdno:      CMDNO_STOP_PULL,
		Uid:        uid,
		StreamName: streamName,
	}

	var resp xrtcStopPullResp
	err = framework.Call("xrtc", req, &resp, cr.LogId)
	fmt.Printf("%+v\n", resp)
	if err != nil {
		cerr := commonErrors.New(commonErrors.ParamErr, "backend process error:"+err.Error())
		writeJsonErrorResponse(cerr, w, cr)
		return
	}

	if resp.ErrNo != 0 {
		cerr := commonErrors.New(commonErrors.NetworkErr,
			fmt.Sprintf("backend process errno: %d", resp.ErrNo))
		writeJsonErrorResponse(cerr, w, cr)
		return
	}

	httpResp :=  &CommonHttpResp{
		ErrNo:  0,
		ErrMsg: "success",
	}

	b, _ := json.Marshal(httpResp)
	cr.Logger.AddNotice("resp", string(b))
	w.Write(b)
}