package main

import (
	"test/src/framework"
	"test/src/action"
)

func init(){
    framework.GActionRouter["/lym/rtc"] = action.NewLrtcClientPushAction()
	framework.GActionRouter["/signaling/stoppush"] = action.NewStopPushAction()
	framework.GActionRouter["/signaling/pull"] = action.NewPullAction()
	framework.GActionRouter["/signaling/stoppull"] = action.NewStopPullAction()

	framework.GActionRouter["/signaling/sendanswer"] = action.NewSendAnswerAction()
	framework.GActionRouter["/signaling/push"] = action.NewPushAction()
}