package lrpc

import (
	"io"
	"fmt"
)

type Resopnse struct {
	Header Header
	Body []byte

}
func ReadResponse(r io.Reader)(resp *Resopnse,err error){
	resp = new(Resopnse)
	if _, err := resp.Header.ReadHeader(r); err != nil {
		return nil,err
	}
	if resp.Header.MagicNum != HEADER_MAGICNUM {
		return nil,fmt.Errorf("invalid magic number %x", resp.Header.MagicNum)
	}
	resp.Body = make([]byte, resp.Header.BodyLen)
	if _, err := io.ReadFull(r, resp.Body); err != nil {
		return nil,err
	}
	return resp,nil
}