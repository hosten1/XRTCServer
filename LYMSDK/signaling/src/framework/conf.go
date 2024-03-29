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
}

var configFile *goconfig.ConfigFile
func LoadConf(confFile string) (*FrameworkConf,error)){
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
	conf.logToStderr,err = configFile.GetValue("log","logToStderr")
    if err != nil {
		return nil,err
	}
	conf.logToFile,err = configFile.GetValue("log","logToFile")
	if err != nil {
		return nil,err
	}	

}