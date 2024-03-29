package framework

import (
	"test/src/goconfig"
)

type FrameworkConf struct{
	logDir string
	logFile string
	logLevel string
	logToStderr bool
	logToFile bool
	httpPort int
	httpStaticDic string
    httpStaticPrefix string

	httpsPort int
	httpsCert string
	httpsKey string

}

var configFile *goconfig.ConfigFile
func LoadConf(confFile string) (*FrameworkConf,error) {
    var err error
	configFile,err = goconfig.LoadConfigFile(confFile)
	if err != nil {
		return nil,err
	}

	conf := &FrameworkConf{}
	conf.logDir,err = configFile.GetValue("log","logDir")
	if err != nil {
		return nil,err
	}
	conf.logFile,err = configFile.GetValue("log","logFile")
	if err != nil {
		return nil,err
	}
	conf.logLevel,err = configFile.GetValue("log","logLevel")
	if err != nil {
		return nil,err
	}
	conf.logToStderr,err = configFile.Bool("log","logToStderr")
    if err != nil {
		return nil,err
	}
	conf.logToFile,err = configFile.Bool("log","logToFile")
	if err != nil {
		return nil,err
	}	
	conf.httpPort,err = configFile.Int("http","port")
    if err != nil {
		return nil,err
	}
	conf.httpStaticDic,err = configFile.GetValue("http","staticDic")
	if err != nil {
		return nil,err
	}	

	conf.httpsPort,err = configFile.Int("https","port")
	if err != nil {
		return nil,err
	}
	conf.httpsCert,err = configFile.GetValue("https","cert")
	if err != nil {
		return nil,err
	}
	conf.httpsKey,err = configFile.GetValue("https","key")
	if err != nil {
		return nil,err
	}
	conf.httpStaticPrefix,err = configFile.GetValue("http","staticPrefix")
	if err != nil {
		return nil,err
	}
  return conf, nil
}

func GetStaticDir() string{
	return gconf.httpStaticDic
}