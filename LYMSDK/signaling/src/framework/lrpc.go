package framework

import(
	"fmt"
	"strings"
    "errors"
	"strconv"
	"time"
	"bytes"
	"encoding/json"

	"test/src/framework/lrpc"
)

var lrtpClients map[string]*lrpc.Client = make(map[string]*lrpc.Client)

func loadLrpc() error {
	sections := configFile.GetSectionList()
	for _, section := range sections {
		if !strings.HasPrefix(section,"lrpc."){
          continue
		}
		mSection, err := configFile.GetSection(section)
		
		if err != nil {
			return err
			
		}
		values,ok := mSection["server"]
		if !ok {
			return errors.New("no server field in  config  file")
			
		}
		arrServer := strings.Split(values,",")
		// 去除arrServer前后存在的空格
		for i, server := range arrServer {
			arrServer[i] = strings.TrimSpace(server)
		}

		client := lrpc.NewClient(arrServer)
		if client == nil {
			// 处理 client 为 nil 的情况
			return errors.New("failed to create lrpc client")
		}
        //  fmt.Println(mSection)
		// 解析超时配置 不是必选
		if values,ok := mSection["connectionTimeout"];ok{
			if connectTimeout,err := strconv.Atoi(values);ok {
				if err == nil {
					client.ConnectTimeout = time.Duration(connectTimeout) * time.Millisecond
				}else{
					// fmt.Println("parse cconnectionout failed " + err.Error())
				}
			}
		}	
		// 解析超时配置 不是必选
		if values,ok := mSection["writeTimeout"];ok{
			if readTimeout,err := strconv.Atoi(values);ok {
				if err == nil {
					client.ReadTimeout = time.Duration(readTimeout) * time.Millisecond
				}else{
					// fmt.Println("parse readTimeout failed " + err.Error())
				}
			}
		}		
		// 解析超时配置 不是必选
		if values,ok := mSection["writeTimeout"];ok{
			if writeTimeout,err := strconv.Atoi(values);ok {
				if err == nil {
					client.WriteTimeout = time.Duration(writeTimeout) * time.Millisecond
				}else{
					// fmt.Println("parse writeTimeout failed " + err.Error())
				}
			}
		}	
		lrtpClients[section] = client
	}

	return nil
}
func Call(serviceName string, request interface{} ,response interface{},logId uint32) error{
   
	fmt.Println("call" +serviceName)

	client,ok := lrtpClients["lrpc."+serviceName]
	if !ok {
		return fmt.Errorf("[%s] service not found",serviceName)
	}
    
    content,err := json.Marshal(request)
	if(err != nil){
		return err
	}
    req := lrpc.NewRequest(bytes.NewReader(content),logId)
	resp,err := client.Do(req)
	if err != nil {
		return err
	}
	
	err = json.Unmarshal(resp.Body,response)
	fmt.Println("Call resp:",resp,response,err)
	if err != nil {
		return err
	}
	


	return nil
}