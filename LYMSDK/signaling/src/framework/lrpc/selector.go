package lrpc


import(
	"net"
	"sync"
	"errors"
)

type ServerSelector interface{
  PickServer()(net.Addr,error)
}
type RoundRobinSelector struct {
	sync.RWMutex
	addrs []net.Addr
	curIndex int
}
func (rrs *RoundRobinSelector) SetServer(servers []string) error{
	if len(servers) == 0 {
		return errors.New(" Servers is nil")
	}
	addrs := make([]net.Addr,len(servers))

	for i,server := range servers {
		tcpAddr,err := net.ResolveTCPAddr("tcp",server)
		if err != nil {
			return err
		}
		addrs[i] = tcpAddr
	}
	rrs.Lock()
	rrs.addrs = addrs
	rrs.Unlock()
	return nil
}

// // PickServer 选择下一个服务器地址。
// func (rrs *RoundRobinSelector) PickServer() (net.Addr, error) {
// 	// 优化了锁的使用，通过更细粒度的控制减少锁的持有时间
// 	rrs.Lock()
// 	defer rrs.Unlock()

// 	if len(rrs.addrs) == 0 {
// 		// 优化错误处理，增加日志记录（需要引入日志库，这里用伪代码表示）
// 		// log.Error("No server to pick")
// 		return nil, errors.New("No server to pick")
// 	}

// 	index := rrs.curIndex
// 	rrs.curIndex++
// 	if rrs.curIndex > len(rrs.addrs) {
// 		rrs.curIndex = 0
// 	}

// 	// 由于在判断addrs长度之后立即返回，这里不需要再获取读锁
// 	return rrs.addrs[index], nil
// }
func (rrs *RoundRobinSelector) PickServer() (net.Addr, error) {
	rrs.Lock()
	defer rrs.Unlock()

	if len(rrs.addrs) == 0 {
		return nil, errors.New("No server to pick")
	}

	index := rrs.curIndex
	rrs.curIndex = (rrs.curIndex + 1) % len(rrs.addrs)
	return rrs.addrs[index], nil
}