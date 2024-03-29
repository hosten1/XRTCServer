package action


import(
	"bytes"
	"encoding/json"
	"strconv"
	"net/http"
	"test/src/commonErrors"
	"test/src/framework"
)

type CommonHttpResp struct {
	ErrNo  int `json:"errNo"`
	ErrMsg  string `json:"errMsg"`
	Data  interface{} `json:"data"`
}

func writeJsonErrorResponse(cerr *commonErrors.Errors, w http.ResponseWriter, cr *framework.CommonRequest) {
	cr.Logger.AddNotice("errNo", strconv.Itoa(cerr.Errno()))
	cr.Logger.AddNotice("errMsg", cerr.ErrMsg())
	cr.Logger.Warningf("request process failed")
	resp := CommonHttpResp{
		ErrNo:  cerr.Errno(),
		ErrMsg: "process error",
	}
	buffer := new(bytes.Buffer)
	json.NewEncoder(buffer).Encode(&resp)
	w.Write(buffer.Bytes())
}