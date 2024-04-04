package lrpc

import(
	"net"
	"time"
	"bufio"
)
const (
	defaultConnectionTimeout = 100 * time.Millisecond
	defaultrReadTimeout = 500 * time.Millisecond
	defaultWriteTimeout = 500 * time.Millisecond
)

type Client struct {
	ConnectTimeout time.Duration
	ReadTimeout time.Duration
	WriteTimeout time.Duration
	selector ServerSelector
}

func NewClient(servers []string) *Client{
   ss := new(RoundRobinSelector)
   ss.SetServer(servers)
	return &Client{
		selector :ss,
	}
}
func (c *Client) readTimeout() time.Duration {
	if c.ReadTimeout == 0 {
		return defaultrReadTimeout
	}
	return c.ReadTimeout
}
func (c *Client) writeTimeout() time.Duration {
	if c.WriteTimeout == 0 {
		return defaultWriteTimeout
	}
	return c.WriteTimeout
}
func (c *Client) connectionTimeout() time.Duration {
	if c.ConnectTimeout == 0 {
		return defaultConnectionTimeout
	}
	return c.ConnectTimeout
}

func (c *Client) Do(req *Request) (*Resopnse, error){
    addr,err := c.selector.PickServer()
	if err != nil {
		return nil,err
	}
	nc, err := net.DialTimeout(addr.Network(),addr.String(),c.connectionTimeout())
	if err != nil {
		 return nil,err
	}
	defer nc.Close()

	nc.SetReadDeadline(time.Now().Add(c.readTimeout()))
	nc.SetWriteDeadline(time.Now().Add(c.writeTimeout()))

	rw := bufio.NewReadWriter(bufio.NewReader(nc),bufio.NewWriter(nc))
	if _, error  := req.Write(rw);error != nil{
		return nil,error
	}
	if err := rw.Writer.Flush(); err != nil {
		return nil, err
	}

	// 此处需要实现具体的请求发送和响应解析逻辑

	return nil,nil
}