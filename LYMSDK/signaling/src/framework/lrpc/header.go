package lrpc


import(
	"encoding/binary"
	"io"
	"errors"
)
const(
	HEADER_SIZE = 36
	HEADER_MAGICNUM=0xfb202404
)

type Header struct{
	Id uint16
	Version uint16
	LogId uint32
	Provider [16]byte
	MagicNum uint32
	Reserved uint32
    BodyLen uint32
}
func (h *Header) Marshal(buf []byte) error {
	// 将 Header 的字段逐个写入到 buf 中
	if len(buf) < HEADER_SIZE {
		return errors.New("No enough buffer for Header")
	}
	// 写入 Id 字段（2 字节）
	binary.LittleEndian.PutUint16(buf[0:2], h.Id)
	// 写入 Version 字段（2 字节）
	binary.LittleEndian.PutUint16(buf[2:4], h.Version)
	// 写入 LogId 字段（4 字节）
	binary.LittleEndian.PutUint32(buf[4:8], h.LogId)
	// 写入 Provider 字段（16 字节）
	copy(buf[8:24], h.Provider[:])
	// 写入 MagicNum 字段（4 字节）
	binary.LittleEndian.PutUint32(buf[24:28], h.MagicNum)
	// 写入 Reserved 字段（4 字节）
	binary.LittleEndian.PutUint32(buf[28:32], h.Reserved)
	// 写入 BodyLen 字段（4 字节）
	binary.LittleEndian.PutUint32(buf[32:36], h.BodyLen)
	return nil

	
}
func (h *Header) Unmarshal(buf []byte)(err error) {
	if len(buf) < HEADER_SIZE {
		return errors.New("header buf too short")
	}
	h.Id = binary.LittleEndian.Uint16(buf[0:2])
	h.Version = binary.LittleEndian.Uint16(buf[2:4])
	h.LogId = binary.LittleEndian.Uint32(buf[4:8])
	copy(h.Provider[:], buf[8:24])
	h.MagicNum = binary.LittleEndian.Uint32(buf[24:28])
	h.Reserved = binary.LittleEndian.Uint32(buf[28:32])
	h.BodyLen = binary.LittleEndian.Uint32(buf[32:36])
	return nil
}
func (h *Header) Write(w io.Writer) (int, error) {
	// 将 Header 的字段逐个写入到 Writer 中
	var buf [HEADER_SIZE]byte
	if err := h.Marshal(buf[:]); err != nil {
		return 0, err
	}
	// 将 buf 中的数据写入到 Writer
	return w.Write(buf[:])
}
func (h *Header)  ReadHeader(r io.Reader)(n int,err error){
	var buf [HEADER_SIZE]byte
	if n,err = io.ReadFull(r,buf[:]);err != nil{
		return 0,err
	}
	if err := h.Unmarshal(buf[:]); err != nil {
		return 0, err
	}
	return n ,nil
}