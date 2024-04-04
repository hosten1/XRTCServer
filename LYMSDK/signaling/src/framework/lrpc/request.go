package lrpc

import(
	"io"
	"bytes"
)

type Request struct {
	Header Header
	Body io.Reader
}
func NewRequest(body io.Reader,logid uint32) (*Request){
	req := new(Request)
	req.Header.LogId = logid
	req.Header.MagicNum = HEADER_MAGICNUM
	if body != nil {
		switch v := body.(type) {
		case *bytes.Reader:{
			req.Header.BodyLen = uint32(v.Len()) 
		}
		default:
			return nil
		}
		req.Body = io.LimitReader(body,int64(req.Header.BodyLen))
		return req
	}
	return nil

}