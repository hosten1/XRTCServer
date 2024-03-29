package main

import (
	"test/src/framework"
	"test/src/action"
)

func init(){
    framework.GActionRouter["/lym/rtc"] = action.NewLrtcClientPushAction()
	framework.GActionRouter["/signaling/push"] = action.NewPushAction()
}