package commonErrors


const (
  NoErr =0
  ParamErr = -1
  NetworkErr = -2
)

type Errors struct{
	errno int
	err string
}

func New(errnoI int,errI string) *Errors{
	return &Errors{
		errno:errnoI,
		err:errI,
	}
}

func (e *Errors) Errno() int{
	return e.errno;
}

func (e *Errors) ErrMsg() string{
	return e.err;
}